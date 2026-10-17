#include "CardGate.hpp"

#include "card.h"
#include "os.h"
#include "prime/CGameState.hpp"
#include "prime/CMain.hpp"
#include "prime/CMemoryCardSys.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CToken.hpp"
#include "prime/CWorld.hpp"
#include "prime/CWorldState.hpp"
#include "settings.hpp"
#include <string.h>

namespace CardGate {
  namespace {
    constexpr s32 kChan = 0;
    constexpr CMemoryCardSys::EMemoryCardPort kPort = CMemoryCardSys::kCS_SlotA;
    constexpr char kFileName[] = "PrimePractice";
    constexpr char kComment[] = "Prime Practice Mod";
    constexpr u32 kBlockSize = 0x2000;
    constexpr int kTimeoutFrames = 60 * 20;
    constexpr s64 kDriverDrainTimeout = 5 * TICKS_PER_SECOND;

    // What the game's own save needs free: first save creates a file plus a backup, later saves just replace it
    constexpr u32 kGameFirstSaveBytes = 0x4000;
    constexpr u32 kGameFirstSaveFiles = 2;
    constexpr u32 kGameResaveBytes = 0x2000;
    constexpr u32 kGameResaveFiles = 1;

    // Same layout the game uses: crc, comment, banner (+palette), icon (+palette), payload
    constexpr u32 kCommentOffset = 4;
    constexpr u32 kCommentSize = 64;
    constexpr u32 kBannerOffset = kCommentOffset + kCommentSize;
    constexpr u32 kBannerBytes = 96 * 32 + 512;
    constexpr u32 kIconBytes = 32 * 32 + 512;
    constexpr u32 kPayloadOffsetWithArt = kBannerOffset + kBannerBytes + kIconBytes;
    constexpr u32 kPayloadOffsetNoArt = kBannerOffset;

    constexpr u32 kMagic = 0x50505356; // 'PPSV'
    constexpr u32 kVersion = 2;
    constexpr u32 kMaxRecordsBytes = 1024;

    // Records are [id u8][length u8][big-endian value]. Unknown ids are skipped and missing ones keep their defaults,
    // so settings can be added or removed without bumping kVersion; it only changes if this encoding does.
    struct PayloadHeader {
      u32 magic;
      u32 version;
      u32 recordsSize;
      u32 recordsCrc;
    };

#define SETTINGS_COUNT_FIELD(id, field) +1
    constexpr u32 kFieldCount = 0 SETTINGS_FIELDS(SETTINGS_COUNT_FIELD);
#undef SETTINGS_COUNT_FIELD
    static_assert(kFieldCount * 6 <= kMaxRecordsBytes);
    static_assert(kPayloadOffsetWithArt + sizeof(PayloadHeader) + kMaxRecordsBytes <= kBlockSize);

    // Bitfields can't be bound to a reference, so each field goes through a local
    template <class Visitor> void visitSettings(Settings &s, Visitor &visitor) {
#define SETTINGS_VISIT_FIELD(id, field) \
  { \
    auto value = s.field; \
    visitor(static_cast<u8>(id), value); \
    if constexpr (Visitor::kLoads) s.field = value; \
  }
      SETTINGS_FIELDS(SETTINGS_VISIT_FIELD)
#undef SETTINGS_VISIT_FIELD
    }

    struct RecordWriter {
      static constexpr bool kLoads = false;
      u8 *out;
      u32 size = 0;
      __attribute__((noinline)) void put(u8 id, u32 value, u8 length) {
        out[size++] = id;
        out[size++] = length;
        for (int shift = (length - 1) * 8; shift >= 0; shift -= 8) out[size++] = value >> shift;
      }
      void putWords(u8 id, const u32 *words, u8 count) {
        out[size++] = id;
        out[size++] = count * 4;
        for (u8 i = 0; i < count; i++) {
          for (int shift = 24; shift >= 0; shift -= 8) out[size++] = words[i] >> shift;
        }
      }
      __attribute__((noinline)) void operator()(u8 id, bool value) { put(id, value, 1); }
      __attribute__((noinline)) void operator()(u8 id, s32 value) { put(id, static_cast<u32>(value), 4); }
    };

    struct RecordReader {
      static constexpr bool kLoads = true;
      const u8 *data;
      u32 size;
      // Value bytes of the record with this id and length, or null
      const u8 *find(u8 id, u8 length) const {
        for (u32 at = 0; at + 2 <= size && at + 2 + data[at + 1] <= size; at += 2 + data[at + 1]) {
          if (data[at] == id && data[at + 1] == length) return data + at + 2;
        }
        return nullptr;
      }
      // The index-th word of a record that is length bytes long
      bool word(u8 id, u8 length, u32 index, u32 &value) const {
        const u8 *p = find(id, length);
        if (!p) return false;
        p += index * 4;
        value = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
        return true;
      }
      __attribute__((noinline)) void operator()(u8 id, bool &value) {
        if (const u8 *p = find(id, 1)) value = p[0] != 0;
      }
      __attribute__((noinline)) void operator()(u8 id, s32 &value) {
        if (const u8 *p = find(id, 4)) value = static_cast<s32>(p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]);
      }
    };

    enum class Step { Idle, Probing, Mounting, Deleting, Creating, Writing, SettingStatus, Reading };
    enum class Op { None, Load, Save };

    Step step = Step::Idle;
    Op op = Op::None;
    int stepFrames = 0;
    int driverCount = 0;
    bool saveWanted = false;
    bool loadDone = false;
    bool mounted = false;
    bool fileOpen = false;
    bool writingArt = false;
    CARDFileInfo file;
    const char *lastMessage = nullptr;

    // What is on the card; written is the snapshot that went into the buffer, which may differ from SETTINGS by the
    // time the write finishes
    Settings saved;
    Settings written;

    // Where Save Anywhere was used. Only applied to the save it was made with, which the play time identifies: the
    // game restores a save at its room's default spawn, and a later save elsewhere must not be overridden by this
    struct Location {
      bool valid = false;
      u32 worldId = 0;
      u32 areaId = 0;
      u32 playTimeCs = 0;
      CTransform4f transform = CTransform4f::Identity();
    };
    constexpr u8 kIdWorld = 100, kIdArea = 101, kIdPlayTime = 102, kIdTransform = 103;
    constexpr u32 kPlayTimeToleranceCs = 100;
    Location location;
    bool settingsWanted = false;
    bool saveIncludesSettings = false;
    bool wasPlaying = false;
    bool sessionPending = false;
    u32 sessionPlayTimeCs = 0;

    // Only allocated while a card operation runs. The SDK's DMA needs 32-byte alignment, which we ensure ourselves
    // rather than relying on the allocator
    u8 *bufferRaw = nullptr;
    u8 *buffer = nullptr;

    u32 crc32(const void *data, u32 len) {
      const u8 *p = static_cast<const u8 *>(data);
      u32 crc = 0xFFFFFFFF;
      for (u32 i = 0; i < len; i++) {
        crc ^= p[i];
        for (int bit = 0; bit < 8; bit++) {
          crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
      }
      return ~crc;
    }

    bool sameSettings(const Settings &a, const Settings &b) { return memcmp(&a, &b, sizeof(Settings)) == 0; }

    bool inGameplay() {
      return gpGameState && g_StateManager.Player() && g_StateManager.GetInitPhase() == CStateManager::kInit_Done;
    }

    void setBusy(bool busy) {
      if (gpMain) gpMain->SetCardBusy(busy);
    }

    // The card is always left unmounted with the work area freed, success or failure
    void finish(const char *message) {
      if (fileOpen) {
        CARDClose(&file);
        fileOpen = false;
      }
      if (mounted) {
        CMemoryCardSys::UnmountCard(kPort);
        mounted = false;
      }
      delete[] bufferRaw;
      bufferRaw = buffer = nullptr;
      if (message) lastMessage = message;
      if (op == Op::Load) {
        loadDone = true;
        // Whatever is in use now (loaded or default) is the baseline for the unsaved-changes indicator
        memcpy(&saved, &SETTINGS, sizeof(Settings));
      }
      step = Step::Idle;
      op = Op::None;
      setBusy(false);
    }

    void fail(const char *message, s32 code) {
      DebugLog("CardGate: %s (%d)\n", message, code);
      // A failed save stays visible as unsaved changes; the user can retry
      finish(message);
    }

    void enter(Step next) {
      step = next;
      stepFrames = 0;
    }

    // Layout a file has according to its card status, so a file written elsewhere still reads right
    u32 payloadOffset(const CARDStat &stat) {
      u32 offset = kBannerOffset;
      if ((stat.bannerFormat & 3) == 1) offset += 96 * 32 + 512;
      else if ((stat.bannerFormat & 3) == 2) offset += 96 * 32 * 2;
      bool palette = false;
      for (int i = 0; i < 8; i++) {
        u32 format = (stat.iconFormat >> (i * 2)) & 3;
        if (format == 0) break;
        if (format == 1) {
          offset += 32 * 32;
          palette = true;
        } else {
          offset += 32 * 32 * 2;
        }
      }
      return palette ? offset + 512 : offset;
    }

    // C8 pixels use GX 8x4 tiles; palettes contain big-endian RGB5A3 entries.
    void artPixel(u8 *image, int width, int x, int y, u8 color) {
      image[((y / 4) * (width / 8) + x / 8) * 32 + (y % 4) * 8 + x % 8] = color;
    }

    void artText(u8 *image, int width, int x, int y, const char *text, int scale) {
      // Five columns per glyph, with bit zero at the top. Only the card labels need these glyphs.
      constexpr char letters[] = "ACEHIMOPRST1";
      constexpr u8 glyphs[][5] = {
          {0x7E, 0x09, 0x09, 0x09, 0x7E}, // A
          {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
          {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
          {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
          {0x41, 0x41, 0x7F, 0x41, 0x41}, // I
          {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
          {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
          {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
          {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
          {0x46, 0x49, 0x49, 0x49, 0x31}, // S
          {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
          {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
      };
      for (; *text; ++text, x += 6 * scale) {
        const char *glyph = letters;
        while (*glyph && *glyph != *text)
          ++glyph;
        if (!*glyph) continue;
        for (int col = 0; col < 5; ++col) {
          for (int row = 0; row < 7; ++row) {
            if (!(glyphs[glyph - letters][col] & (1 << row))) continue;
            for (int dy = 0; dy < scale; ++dy)
              for (int dx = 0; dx < scale; ++dx)
                artPixel(image, width, x + col * scale + dx, y + row * scale + dy, 3);
          }
        }
      }
    }

    void artBackground(u8 *image, int width) {
      for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < width; ++x) {
          bool border = x == 0 || x == width - 1 || y == 0 || y == 31;
          artPixel(image, width, x, y, border ? 2 : 1);
        }
      }
      u8 *palette = image + width * 32;
      memset(palette, 0, 512);
      constexpr u16 colors[] = {0x8000, 0x8423, 0x82F7, 0xFFFF}; // black, navy, teal, white
      for (u32 i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
        palette[i * 2] = colors[i] >> 8;
        palette[i * 2 + 1] = colors[i];
      }
    }

    // Independent artwork avoids borrowing game texture/token storage.
    bool writeArt(u8 *out) {
      artBackground(out, 96);
      artText(out, 96, 33, 6, "PRIME", 1);
      artText(out, 96, 24, 19, "PRACTICE", 1);
      out += kBannerBytes;
      artBackground(out, 32);
      artText(out, 32, 5, 9, "P1", 2);
      return true;
    }

    // Returns whether the banner and icon were included
    bool buildFile() {
      memset(buffer, 0, kBlockSize);
      memcpy(buffer + kCommentOffset, kComment, sizeof(kComment) - 1);
      bool art = writeArt(buffer + kBannerOffset);
      if (!art) memset(buffer + kBannerOffset, 0, kBannerBytes + kIconBytes);

      memcpy(&written, saveIncludesSettings ? &SETTINGS : &saved, sizeof(Settings));
      u8 *payload = buffer + (art ? kPayloadOffsetWithArt : kPayloadOffsetNoArt);
      RecordWriter writer{payload + sizeof(PayloadHeader)};
      visitSettings(written, writer);
      if (location.valid) {
        writer.put(kIdWorld, location.worldId, 4);
        writer.put(kIdArea, location.areaId, 4);
        writer.put(kIdPlayTime, location.playTimeCs, 4);
        u32 words[12];
        memcpy(words, location.transform.matrix, sizeof(words));
        writer.putWords(kIdTransform, words, 12);
      }
      PayloadHeader header{kMagic, kVersion, writer.size, crc32(writer.out, writer.size)};
      memcpy(payload, &header, sizeof(header));
      return art;
    }

    bool fileExists(const char *name) {
      CARDFileInfo info;
      if (CARDOpen(kChan, name, &info) != CARD_RESULT_READY) return false;
      CARDClose(&info);
      return true;
    }

    bool hasEnoughSpace(bool replacingOurs) {
      s32 freeBytes, freeFiles;
      if (CARDFreeBlocks(kChan, &freeBytes, &freeFiles) != CARD_RESULT_READY) return false;
      if (replacingOurs) {
        freeBytes += kBlockSize;
        freeFiles += 1;
      }
      bool hasGameSave = fileExists("MetroidPrime A") || fileExists("MetroidPrime B");
      u32 leaveBytes = hasGameSave ? kGameResaveBytes : kGameFirstSaveBytes;
      u32 leaveFiles = hasGameSave ? kGameResaveFiles : kGameFirstSaveFiles;
      return freeBytes >= static_cast<s32>(kBlockSize + leaveBytes) && freeFiles >= static_cast<s32>(1 + leaveFiles);
    }

    void startCreate() {
      writingArt = buildFile();
      if (!writingArt) DebugLog("CardGate: banner/icon unavailable, writing without\n");
      s32 result = CARDCreateAsync(kChan, kFileName, kBlockSize, &file, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not create the settings file", result);
      enter(Step::Creating);
    }

    void startSave() {
      bool exists = fileExists(kFileName);
      if (!hasEnoughSpace(exists)) {
        return fail("Not enough free memory card space", 0);
      }
      if (!exists) return startCreate();
      s32 result = CARDDeleteAsync(kChan, kFileName, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not replace the old settings file", result);
      enter(Step::Deleting);
    }

    void startLoad() {
      s32 result = CARDOpen(kChan, kFileName, &file);
      if (result == CARD_RESULT_NOFILE) return finish("No saved settings");
      if (result != CARD_RESULT_READY) return fail("Could not open the settings file", result);
      fileOpen = true;

      CARDStat stat;
      result = CARDGetStatus(kChan, file.fileNo, &stat);
      if (result != CARD_RESULT_READY || stat.length != kBlockSize) {
        return fail("Settings file has an unexpected size, using defaults", result);
      }
      DCInvalidateRange(buffer, kBlockSize);
      result = CARDReadAsync(&file, buffer, kBlockSize, 0, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not read the settings file", result);
      enter(Step::Reading);
    }

    void finishLoad() {
      CARDStat stat;
      if (CARDGetStatus(kChan, file.fileNo, &stat) != CARD_RESULT_READY) {
        return fail("Could not read the settings file status, using defaults", 0);
      }
      u32 offset = payloadOffset(stat);
      if (offset + sizeof(PayloadHeader) > kBlockSize) {
        return fail("Settings file is not recognized, using defaults", 0);
      }
      PayloadHeader header;
      memcpy(&header, buffer + offset, sizeof(header));
      const u8 *data = buffer + offset + sizeof(header);
      if (header.magic != kMagic || header.version != kVersion || header.recordsSize > kMaxRecordsBytes ||
          offset + sizeof(header) + header.recordsSize > kBlockSize || header.recordsCrc != crc32(data, header.recordsSize)) {
        return fail("Saved settings are from a different version, using defaults", 0);
      }
      RecordReader reader{data, header.recordsSize};
      visitSettings(SETTINGS, reader);
      u32 words[12];
      location.valid = reader.word(kIdWorld, 4, 0, location.worldId) && reader.word(kIdArea, 4, 0, location.areaId) &&
                       reader.word(kIdPlayTime, 4, 0, location.playTimeCs);
      for (u32 i = 0; i < 12 && location.valid; i++) location.valid = reader.word(kIdTransform, 48, i, words[i]);
      if (location.valid) memcpy(location.transform.matrix, words, sizeof(words));
      memcpy(&saved, &SETTINGS, sizeof(Settings));
      finish("Loaded saved settings");
    }

    void startStatus() {
      CARDStat stat;
      s32 result = CARDGetStatus(kChan, file.fileNo, &stat);
      if (result != CARD_RESULT_READY) return fail("Could not read the settings file status", result);
      stat.commentAddr = kCommentOffset;
      stat.iconAddr = kBannerOffset;
      stat.bannerFormat = (stat.bannerFormat & ~3) | (writingArt ? 1 : 0);
      stat.iconFormat = writingArt ? 1 : 0; // slot 0 = C8, other slots none
      stat.iconSpeed = writingArt ? 2 : 0;
      result = CARDSetStatusAsync(kChan, file.fileNo, &stat, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not finish the settings file", result);
      enter(Step::SettingStatus);
    }

    u32 currentAreaAssetId() {
      CWorld *world = g_StateManager.GetWorld();
      if (!world) return 0;
      return world->areas()->ptr[gpGameState->CurrentWorldState().x4_areaId.id].ptr->IGetAreaAssetId();
    }

    u32 playTimeCs() { return static_cast<u32>(gpGameState->PlayTime() * 100.0); }

    void applyLocation() {
      sessionPending = false;
      u32 diff = sessionPlayTimeCs > location.playTimeCs ? sessionPlayTimeCs - location.playTimeCs
                                                         : location.playTimeCs - sessionPlayTimeCs;
      if (!location.valid || diff > kPlayTimeToleranceCs || gpGameState->MLVL() != location.worldId ||
          currentAreaAssetId() != location.areaId) {
        return;
      }
      CPlayer *player = g_StateManager.Player();
      *player->getTransform() = location.transform;
      *player->GetVelocity() = CVector3f{};
      *player->GetAngularVelocity() = CVector3f{};
      lastMessage = "Restored Save Anywhere position";
    }

    void begin(Op newOp) {
      op = newOp;
      saveIncludesSettings = settingsWanted;
      settingsWanted = false;
      setBusy(true);
      bufferRaw = new u8[kBlockSize + 32];
      if (!bufferRaw) return fail("Out of memory", 0);
      buffer = reinterpret_cast<u8 *>((reinterpret_cast<u32>(bufferRaw) + 31) & ~31u);
      enter(Step::Probing);
    }

    void startMount() {
      s32 memSize, sectorSize;
      s32 result = CARDProbeEx(kChan, &memSize, &sectorSize);
      if (result == CARD_RESULT_BUSY) return;
      if (result == CARD_RESULT_NOCARD) return fail("No memory card in slot A", result);
      if (result != CARD_RESULT_READY) return fail("Memory card in slot A is not usable", result);
      if (sectorSize != static_cast<s32>(kBlockSize)) return fail("Memory card has an unsupported sector size", 0);

      mounted = true;
      result = CMemoryCardSys::MountCard(kPort);
      if (result != CARD_RESULT_READY) return fail("Could not mount", result);
      enter(Step::Mounting);
    }

    // Anything but busy on a step's command ends that step
    void advance(bool draining) {
      if (!draining && ++stepFrames > kTimeoutFrames) return fail("Memory card op timed out", 0);
      if (step == Step::Probing) return startMount();

      s32 result = CARDGetResultCode(kChan);
      if (result == CARD_RESULT_BUSY) return;
      if (result != CARD_RESULT_READY) return fail("Memory card op failed", result);

      switch (step) {
      case Step::Mounting:
        return op == Op::Load ? startLoad() : startSave();
      case Step::Deleting:
        return startCreate();
      case Step::Creating:
        fileOpen = true;
        DCFlushRange(buffer, kBlockSize);
        result = CARDWriteAsync(&file, buffer, kBlockSize, 0, nullptr);
        if (result != CARD_RESULT_READY) return fail("Could not write", result);
        return enter(Step::Writing);
      case Step::Writing:
        return startStatus();
      case Step::SettingStatus:
        memcpy(&saved, &written, sizeof(Settings));
        return finish("Settings saved");
      case Step::Reading:
        return finishLoad();
      default:
        return;
      }
    }
  } // namespace

  void tick() {
    bool playing = inGameplay();
    if (playing && !wasPlaying) {
      sessionPlayTimeCs = playTimeCs();
      sessionPending = true;
    }
    wasPlaying = playing;
    if (playing && sessionPending && loadDone) applyLocation();

    if (step != Step::Idle) return advance(false);
    // Driver wins: it owns the card for as long as it exists
    if (driverCount > 0) return;

    if (saveWanted) {
      saveWanted = false;
      lastMessage = "Saving settings...";
      return begin(Op::Save);
    }
    if (!loadDone && inGameplay()) begin(Op::Load);
  }

  void requestSave() { saveWanted = settingsWanted = true; }

  void saveLocationWithGame() {
    // Until the file has been read, saved isn't known and writing would replace the stored settings with defaults
    if (!loadDone || !inGameplay()) return;
    CPlayer *player = g_StateManager.Player();
    location.valid = true;
    location.worldId = gpGameState->MLVL();
    location.areaId = currentAreaAssetId();
    location.playTimeCs = playTimeCs();
    location.transform = *player->getTransform();
    saveWanted = true;
  }
  bool saveRequested() { return saveWanted; }
  bool busy() { return step != Step::Idle; }
  bool dirty() { return loadDone && !sameSettings(saved, SETTINGS); }
  const char *message() { return lastMessage; }

  void driverConstructing() {
    if (step != Step::Idle) {
      s64 start = OSGetTime();
      while (step != Step::Idle && OSGetTime() - start < kDriverDrainTimeout) advance(true);
      if (step != Step::Idle) fail("Memory card operation interrupted", 0);
    }
    driverCount++;
  }

  void driverDestroyed() {
    if (driverCount > 0) driverCount--;
  }
} // namespace CardGate
