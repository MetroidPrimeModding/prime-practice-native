
#pragma once

#include "HookStubConstants.h"
#include "types.h"

extern "C" void HookStub();

// A hook function for powerpc 32 (for the gamecube):
// It will, at patch time:
// - overwrite the first bytes of the target address with a branch to the hook function
// - save the overwritten bytes in a trampoline (this trampoline should be stored in a statically allocated buffer)
//
// At hook time, it will:
// - save all registers
// - call the hook function, passing the saved registers and a pointer to the trampoline
// - restore all registers
// - jump to the trampoline to execute the overwritten bytes and continue execution
struct Hook {
  struct Registers {
    u32 gpr[32];
    u32 lr;
    u32 cr;
    u32 ctr;
    u32 xer;
    u32 pad;
    double fpr[32];
    bool earlyReturn{false};

    template <typename T>
    T GetCallArg(int index) const {
      if constexpr (sizeof(T) == 4) {
        return (T)(gpr[3 + index]);
      } else if constexpr (sizeof(T) == 8) {
        return (T)((static_cast<u64>(gpr[3 + index]) << 32) | gpr[3 + index + 1]);
      } else {
        static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported argument size");
      }
    }

    template <typename T>
    T GetThis() {
      return GetCallArg<T>(0);
    }

    template <typename T>
    void SetReturnValue(T value) {
      if constexpr (sizeof(T) == 4) {
        gpr[3] = static_cast<u32>(value);
      } else if constexpr (sizeof(T) == 8) {
        gpr[3] = static_cast<u32>((static_cast<u64>(value) >> 32) & 0xFFFFFFFF);
        gpr[4] = static_cast<u32>(static_cast<u64>(value) & 0xFFFFFFFF);
      } else {
        static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Unsupported return value size");
      }
    }
  } __attribute__((aligned(8)));

  static constexpr size_t kTrampolineInstructionCount = 2;
  static constexpr size_t kTrampolineSize = sizeof(u32) * kTrampolineInstructionCount;

  static constexpr size_t kFrameHeaderSize = 8;
  static constexpr size_t kRegistersOffset = kFrameHeaderSize;
  static constexpr size_t kFrameSize = ((kFrameHeaderSize + sizeof(Registers) + 15) / 16) * 16;
  static constexpr size_t kEntryInstructionCount = 3;
  static constexpr size_t kEntrySize = kEntryInstructionCount * sizeof(u32);

  static constexpr size_t kRegsGprOffset = offsetof(Registers, gpr);
  static constexpr size_t kRegsLrOffset = offsetof(Registers, lr);
  static constexpr size_t kRegsCrOffset = offsetof(Registers, cr);
  static constexpr size_t kRegsCtrOffset = offsetof(Registers, ctr);
  static constexpr size_t kRegsXerOffset = offsetof(Registers, xer);
  static constexpr size_t kRegsFprOffset = offsetof(Registers, fpr);
  static constexpr size_t earlyReturnOffset = offsetof(Registers, earlyReturn);

  static_assert(kRegistersOffset == HOOK_REGS_OFFSET, "HookStub constants must match stack layout.");
  static_assert(kFrameSize == HOOK_FRAME_SIZE, "HookStub constants must match frame size.");
  static_assert(kRegsGprOffset == HOOK_REG_GPR_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(kRegsLrOffset == HOOK_REG_LR_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(kRegsCrOffset == HOOK_REG_CR_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(kRegsCtrOffset == HOOK_REG_CTR_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(kRegsXerOffset == HOOK_REG_XER_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(kRegsFprOffset == HOOK_REG_FPR_OFFSET, "HookStub constants must match Registers layout.");
  static_assert(earlyReturnOffset == HOOK_REG_EARLY_RET_OFFSET, "HookStub constants must match Registers layout.");

  using HookFn = void (*)(Registers *regs, void *trampoline);

  struct HookContext {
    HookFn hook;
    void *trampoline;
  };

  static_assert(offsetof(HookContext, hook) == HOOK_CTX_HOOK_OFFSET, "HookStub constants must match HookContext layout.");
  static_assert(offsetof(HookContext, trampoline) == HOOK_CTX_TRAMP_OFFSET, "HookStub constants must match HookContext layout.");

  Hook(const char* name, void *target, HookFn hookFn);

  // void *GetTrampoline() const;
  // void *GetEntry() const;
  bool IsInstalled() const;

private:
  bool Install();

  alignas(4) u32 entry[kEntryInstructionCount]{0xFFFFFFFF};
  alignas(4) u32 trampoline[kTrampolineInstructionCount]{0xFFFFFFFF};
  HookContext context;
  const char* name;
  void *target = nullptr;
  HookFn hookFn = nullptr;
  bool installed = false;
};

#define DEFINE_HOOK(target, name) \
  void Hook_##name##_HookFunc(Hook::Registers *regs, void *trampoline); \
  Hook g_##name##_Hook(#name, (void *)(target), Hook_##name##_HookFunc); \
  void Hook_##name##_HookFunc(Hook::Registers *regs, void *trampoline)
