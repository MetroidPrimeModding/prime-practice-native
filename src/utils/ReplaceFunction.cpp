#include "ReplaceFunction.hpp"

namespace ReplaceFunctionHookPool {
  void FlushCode(void *ptr, size_t size) {
    DCFlushRange(ptr, size);
    ICInvalidateRange(ptr, size);
  }

  bool IsRelBranchReachable(const void *from, const void *to) {
    const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
    return diff >= -0x02000000 && diff <= 0x01FFFFFC;
  }

  u32 MakeBranchRel(const void *from, const void *to, bool link) {
    const int32_t diff = static_cast<int32_t>(reinterpret_cast<const u8 *>(to) - reinterpret_cast<const u8 *>(from));
    const u32 li = static_cast<u32>(diff) & 0x03FFFFFC;
    return 0x48000000 | li | (link ? 1u : 0u);
  }

  void BuildTrampoline(u32 *trampolineWords, const u32 *targetInstr) {
    // DebugLog("Trampoline : %p, TargetInstr: %p\n", trampolineWords, targetInstr);
    trampolineWords[0] = *targetInstr;
    trampolineWords[1] =
        MakeBranchRel(&trampolineWords[1], reinterpret_cast<const u8 *>(targetInstr) + sizeof(u32), false);
  }

  constexpr size_t TrampolineSize = 2;
  struct ReplacementHookTrampoline {
    void* originalAddress{nullptr};
    u32 trampolineWords[TrampolineSize]{0};
  };

  constexpr size_t HookPoolSize = 256;
  constexpr size_t MaxHooks = HookPoolSize / (sizeof(ReplacementHookTrampoline));

  ReplacementHookTrampoline hookPool[MaxHooks]{};
  u32 currentHookIndex{0};
  static_assert(sizeof(hookPool) <= HookPoolSize, "Hook pool size mismatch");

  ReplacementHookTrampoline* AllocateHookTrampoline() {
    if (currentHookIndex >= MaxHooks) {
      DebugLog("ReplaceFunctionHookPool: Out of hook trampolines!\n");
      return nullptr;
    }
    return &hookPool[currentHookIndex++];
  }

  ReplacementHookTrampoline* findExistingHook(const void *originalPtr) {
    for (u32 i = 0; i < currentHookIndex; i++) {
      auto trampoline = &hookPool[i];
      if (trampoline->originalAddress == originalPtr) {
        return trampoline;
      }
    }
    return nullptr;
  }

  void * Install(const char* name, void* originalPtr, void* replacementPtr) {
    u32 *targetInstr = reinterpret_cast<u32 *>(originalPtr);

    // check if original instruction is a branch to something that could be inside the hook pool
    ReplacementHookTrampoline* originalFnTrampoline = findExistingHook(originalPtr);
    if (originalFnTrampoline != nullptr) {
      DebugLog("Hook #%d %s already has trampoline at %p\n", currentHookIndex, name, originalFnTrampoline->trampolineWords);
    } else {
      originalFnTrampoline = AllocateHookTrampoline();
      if (originalFnTrampoline != nullptr) {
        originalFnTrampoline->originalAddress = originalPtr;
        BuildTrampoline(originalFnTrampoline->trampolineWords, targetInstr);
        FlushCode(originalFnTrampoline, TrampolineSize);
      }
    }

    if (originalFnTrampoline == nullptr) {
      DebugLog("ReplaceFunctionHookPool: Failed to allocate trampoline for hook at %p to %p\n", originalPtr,
               replacementPtr);
      return nullptr;
    }

    if (!IsRelBranchReachable(targetInstr, replacementPtr) ||
        !IsRelBranchReachable(originalFnTrampoline->trampolineWords + 1, static_cast<u32*>(originalPtr) + 1)) {
      DebugLog("ReplaceFunctionHookPool: Branch not reachable for hook at %p to %p\n", originalPtr,
               replacementPtr);
      return nullptr;
    }

    *targetInstr = MakeBranchRel(targetInstr, replacementPtr, false);
    FlushCode(targetInstr, sizeof(u32));

    DebugLog("Hook #%d %s installed at %p -> %p (trampoline: %p)\n", currentHookIndex, name, originalPtr, replacementPtr, originalFnTrampoline->trampolineWords);
    return originalFnTrampoline->trampolineWords;
  }
};
