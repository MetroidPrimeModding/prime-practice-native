#include "Hook.hpp"

#include "os.h"

#include <gccore.h>

void FlushCode(void *ptr, size_t size) {
  DCFlushRange(ptr, size);
  ICInvalidateRange(ptr, size);
}

constexpr u16 Hi16(u32 value) { return static_cast<u16>(value >> 16); }
static constexpr u16 Lo16(u32 value) { return static_cast<u16>(value & 0xFFFF); }

bool IsRelBranchReachable(const void *from, const void *to) {
  const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
  return diff >= -0x02000000 && diff <= 0x01FFFFFC;
}

u32 MakeBranchRel(const void *from, const void *to, bool link) {
  const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
  const u32 li = static_cast<u32>(diff) & 0x03FFFFFC;
  return 0x48000000 | li | (link ? 1u : 0u);
}

u32 MakeAddis(u8 rt, u8 ra, u16 imm) {
  return (15u << 26) | (static_cast<u32>(rt) << 21) | (static_cast<u32>(ra) << 16) | imm;
}

u32 MakeOri(u8 rs, u8 ra, u16 imm) {
  return (24u << 26) | (static_cast<u32>(rs) << 21) | (static_cast<u32>(ra) << 16) | imm;
}

u32 MakeLwz(u8 rt, u8 ra, s16 imm) {
  return (32u << 26) | (static_cast<u32>(rt) << 21) | (static_cast<u32>(ra) << 16) | static_cast<u16>(imm);
}

u32 MakeStw(u8 rs, u8 ra, s16 imm) {
  return (36u << 26) | (static_cast<u32>(rs) << 21) | (static_cast<u32>(ra) << 16) | static_cast<u16>(imm);
}

void BuildTrampoline(u32 *trampolineWords, const u32 *targetInstr) {
  // OSReport("Trampoline : %p, TargetInstr: %p\n", trampolineWords, targetInstr);
  trampolineWords[0] = *targetInstr;
  trampolineWords[1] =
      MakeBranchRel(&trampolineWords[1], reinterpret_cast<const u8 *>(targetInstr) + sizeof(u32), false);
}

void BuildEntry(u32 *entryWords, Hook::HookFn hookFn, void *trampoline) {
  constexpr int kEntryTargetStackOffset = HOOK_REGS_OFFSET + HOOK_REG_TARGET_OFFSET - HOOK_FRAME_SIZE;
  constexpr int kEntryTrampolineStackOffset = HOOK_REGS_OFFSET + HOOK_REG_TRAMPOLINE_OFFSET - HOOK_FRAME_SIZE;
  constexpr int kEntrySavedR12Offset =
      HOOK_REGS_OFFSET + HOOK_REG_GPR_OFFSET + (12 * static_cast<int>(sizeof(u32))) - HOOK_FRAME_SIZE;
  static_assert(kEntryTargetStackOffset >= -32768 && kEntryTargetStackOffset <= 32767, "Entry context offset out of range.");
  static_assert(kEntryTrampolineStackOffset >= -32768 && kEntryTrampolineStackOffset <= 32767, "Entry context offset out of range.");
  static_assert(kEntrySavedR12Offset >= -32768 && kEntrySavedR12Offset <= 32767, "Entry save offset out of range.");
  const u32 targetAddr = reinterpret_cast<u32>(hookFn);
  const u32 trampolineAddr = reinterpret_cast<u32>(trampoline);
  entryWords[0] = MakeStw(12, 1, kEntrySavedR12Offset);

  entryWords[1] = MakeAddis(12, 0, Hi16(targetAddr));
  entryWords[2] = MakeOri(12, 12, Lo16(targetAddr));
  entryWords[3] = MakeStw(12, 1, kEntryTargetStackOffset);

  entryWords[4] = MakeAddis(12, 0, Hi16(trampolineAddr));
  entryWords[5] = MakeOri(12, 12, Lo16(trampolineAddr));
  entryWords[6] = MakeStw(12, 1, kEntryTrampolineStackOffset);

  entryWords[7] = MakeLwz(12, 1, kEntrySavedR12Offset);
  entryWords[8] = MakeBranchRel(&entryWords[8], reinterpret_cast<void *>(&HookStub), false);
}

Hook::Hook(const char *name, void *target, HookFn hookFn) : name(name), target(target), hookFn(hookFn) {
  Install();
}

bool Hook::Install() {
  if (installed || target == nullptr || hookFn == nullptr) {
    return false;
  }

  u32 *targetInstr = reinterpret_cast<u32 *>(target);

  if (!IsRelBranchReachable(targetInstr, entry) || !IsRelBranchReachable(entry, reinterpret_cast<void *>(&HookStub)) ||
      !IsRelBranchReachable(trampoline + 1, reinterpret_cast<u8 *>(target) + sizeof(u32))) {
    return false;
  }

  BuildTrampoline(trampoline, targetInstr);
  BuildEntry(entry, hookFn, trampoline);

  FlushCode(trampoline, sizeof(trampoline));
  FlushCode(entry, sizeof(entry));

  *targetInstr = MakeBranchRel(targetInstr, entry, false);
  FlushCode(targetInstr, sizeof(u32));

  installed = true;
  OSReport("Hook %s installed at %p to %p\n", name, target, (void *)hookFn);
  return true;
}
// void *Hook::GetTrampoline() const { return (void *)context.trampoline; }
// void *Hook::GetEntry() const { return (void *)entry; }
bool Hook::IsInstalled() const { return installed; }
