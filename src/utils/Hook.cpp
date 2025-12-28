#include "Hook.hpp"

#include "os.h"

#include <gccore.h>

void FlushCode(void *ptr, size_t size) {
  DCFlushRange(ptr, size);
  ICInvalidateRange(ptr, size);
}

static constexpr u16 Hi16(u32 value) { return static_cast<u16>(value >> 16); }
static constexpr u16 Lo16(u32 value) { return static_cast<u16>(value & 0xFFFF); }

static bool IsRelBranchReachable(const void *from, const void *to) {
  const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
  return diff >= -0x02000000 && diff <= 0x01FFFFFC;
}

static u32 MakeBranchRel(const void *from, const void *to, bool link) {
  const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
  const u32 li = static_cast<u32>(diff) & 0x03FFFFFC;
  return 0x48000000 | li | (link ? 1u : 0u);
}

static u32 MakeAddis(u8 rt, u8 ra, u16 imm) {
  return (15u << 26) | (static_cast<u32>(rt) << 21) | (static_cast<u32>(ra) << 16) | imm;
}

static u32 MakeOri(u8 rs, u8 ra, u16 imm) {
  return (24u << 26) | (static_cast<u32>(rs) << 21) | (static_cast<u32>(ra) << 16) | imm;
}

static void BuildTrampoline(u32 *trampolineWords, const u32 *targetInstr) {
  // OSReport("Trampoline : %p, TargetInstr: %p\n", trampolineWords, targetInstr);
  trampolineWords[0] = *targetInstr;
  trampolineWords[1] =
      MakeBranchRel(&trampolineWords[1], reinterpret_cast<const u8 *>(targetInstr) + sizeof(u32), false);
}

static void BuildEntry(u32 *entryWords, const Hook::HookContext *context) {
  // OSReport("Entry: %p, HookContext: %p\n", entryWords, context);
  const u32 contextAddr = static_cast<u32>(reinterpret_cast<uintptr_t>(context));
  entryWords[0] = MakeAddis(12, 0, Hi16(contextAddr));
  entryWords[1] = MakeOri(12, 12, Lo16(contextAddr));
  entryWords[2] = MakeBranchRel(&entryWords[2], reinterpret_cast<void *>(&HookStub), false);
}

Hook::Hook(const char* name, void *target, HookFn hookFn) : name(name), target(target), hookFn(hookFn), context{} {
  Install();
}

bool Hook::Install() {
  if (installed || target == nullptr || hookFn == nullptr) {
    return false;
  }
  context.hook = hookFn;
  context.trampoline = trampoline;

  u32 *targetInstr = reinterpret_cast<u32 *>(target);

  if (!IsRelBranchReachable(targetInstr, entry) || !IsRelBranchReachable(entry, reinterpret_cast<void *>(&HookStub)) ||
      !IsRelBranchReachable(trampoline + sizeof(u32), reinterpret_cast<u8 *>(target) + sizeof(u32))) {
    return false;
  }

  BuildTrampoline(trampoline, targetInstr);
  BuildEntry(entry, &context);

  *targetInstr = MakeBranchRel(targetInstr, entry, false);

  FlushCode(targetInstr, sizeof(u32));
  FlushCode(trampoline, kTrampolineSize);
  DCFlushRange(&context, sizeof(HookContext));
  FlushCode(entry, kEntrySize);

  installed = true;
  OSReport("Hook %s installed at %p to %p\n", name, target, (void *)hookFn);
  return true;
}
// void *Hook::GetTrampoline() const { return (void *)context.trampoline; }
// void *Hook::GetEntry() const { return (void *)entry; }
bool Hook::IsInstalled() const { return installed; }
