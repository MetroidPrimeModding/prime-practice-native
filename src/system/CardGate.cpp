#include "CardGate.hpp"

#include "card.h"
#include "os.h"
#include "prime/CGameState.hpp"
#include "prime/CMain.hpp"
#include "prime/CMemoryCardSys.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CToken.hpp"
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
    constexpr u32 kVersion = 1;

    struct PayloadHeader {
      u32 magic;
      u32 version;
      u32 settingsSize;
      u32 settingsCrc;
    };
    static_assert(kPayloadOffsetWithArt + sizeof(PayloadHeader) + sizeof(Settings) <= kBlockSize);

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

    alignas(32) u8 buffer[kBlockSize];

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

    // Copies the game's own save banner and icon so the file looks like a Metroid Prime save in the card manager
    bool writeArt(u8 *out) {
      CToken banner = gpSimplePool->GetObj("TXTR_SaveBanner");
      CToken icon = gpSimplePool->GetObj("TXTR_SaveIcon0");
      CTexture *bannerTex = banner.texture();
      CTexture *iconTex = icon.texture();
      if (!bannerTex || !iconTex) return false;
      if (bannerTex->texelFormat() != CTexture::kTF_C8 || iconTex->texelFormat() != CTexture::kTF_C8) return false;
      if (bannerTex->width() != 96 || bannerTex->height() != 32 || iconTex->width() != 32 || iconTex->height() != 32) {
        return false;
      }
      const void *bannerPixels = bannerTex->GetConstBitMapData(0);
      const void *iconPixels = iconTex->GetConstBitMapData(0);
      const u16 *bannerPalette = bannerTex->paletteEntries();
      const u16 *iconPalette = iconTex->paletteEntries();
      if (!bannerPixels || !iconPixels || !bannerPalette || !iconPalette) return false;

      memcpy(out, bannerPixels, 96 * 32);
      memcpy(out + 96 * 32, bannerPalette, 512);
      memcpy(out + kBannerBytes, iconPixels, 32 * 32);
      memcpy(out + kBannerBytes + 32 * 32, iconPalette, 512);
      return true;
    }

    // Returns whether the banner and icon were included
    bool buildFile() {
      memset(buffer, 0, sizeof(buffer));
      memcpy(buffer + kCommentOffset, kComment, sizeof(kComment) - 1);
      bool art = writeArt(buffer + kBannerOffset);
      if (!art) memset(buffer + kBannerOffset, 0, kBannerBytes + kIconBytes);

      memcpy(&written, &SETTINGS, sizeof(Settings));
      PayloadHeader header{kMagic, kVersion, sizeof(Settings), crc32(&written, sizeof(Settings))};
      u8 *payload = buffer + (art ? kPayloadOffsetWithArt : kPayloadOffsetNoArt);
      memcpy(payload, &header, sizeof(header));
      memcpy(payload + sizeof(header), &written, sizeof(Settings));
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
      if (result != CARD_RESULT_READY) return fail("Could not create the settings file on the memory card", result);
      enter(Step::Creating);
    }

    void startSave() {
      bool exists = fileExists(kFileName);
      if (!hasEnoughSpace(exists)) {
        return fail("Not enough free space on the memory card to save settings without affecting game saves", 0);
      }
      if (!exists) return startCreate();
      s32 result = CARDDeleteAsync(kChan, kFileName, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not replace the old settings file", result);
      enter(Step::Deleting);
    }

    void startLoad() {
      s32 result = CARDOpen(kChan, kFileName, &file);
      if (result == CARD_RESULT_NOFILE) return finish("No saved settings on the memory card");
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
      if (offset + sizeof(PayloadHeader) + sizeof(Settings) > kBlockSize) {
        return fail("Settings file is not recognized, using defaults", 0);
      }
      PayloadHeader header;
      memcpy(&header, buffer + offset, sizeof(header));
      const u8 *data = buffer + offset + sizeof(header);
      if (header.magic != kMagic || header.version != kVersion || header.settingsSize != sizeof(Settings) ||
          header.settingsCrc != crc32(data, sizeof(Settings))) {
        return fail("Saved settings are from a different version, using defaults", 0);
      }
      memcpy(&SETTINGS, data, sizeof(Settings));
      memcpy(&saved, &SETTINGS, sizeof(Settings));
      finish("Loaded saved settings from the memory card");
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

    void begin(Op newOp) {
      op = newOp;
      setBusy(true);
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
      if (result != CARD_RESULT_READY) return fail("Could not mount the memory card", result);
      enter(Step::Mounting);
    }

    // Anything but busy on a step's command ends that step
    void advance(bool draining) {
      if (!draining && ++stepFrames > kTimeoutFrames) return fail("Memory card operation timed out", 0);
      if (step == Step::Probing) return startMount();

      s32 result = CARDGetResultCode(kChan);
      if (result == CARD_RESULT_BUSY) return;
      if (result != CARD_RESULT_READY) return fail("Memory card operation failed", result);

      switch (step) {
      case Step::Mounting:
        return op == Op::Load ? startLoad() : startSave();
      case Step::Deleting:
        return startCreate();
      case Step::Creating:
        fileOpen = true;
        DCFlushRange(buffer, kBlockSize);
        result = CARDWriteAsync(&file, buffer, kBlockSize, 0, nullptr);
        if (result != CARD_RESULT_READY) return fail("Could not write the settings file", result);
        return enter(Step::Writing);
      case Step::Writing:
        return startStatus();
      case Step::SettingStatus:
        memcpy(&saved, &written, sizeof(Settings));
        return finish("Settings saved to the memory card");
      case Step::Reading:
        return finishLoad();
      default:
        return;
      }
    }
  } // namespace

  void tick() {
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

  void requestSave() { saveWanted = true; }
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
