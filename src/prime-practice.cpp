#include "PracticeMod.hpp"
#include "utils/ReplaceFunction.hpp"
#include <os.h>
#include <prime/CFontEndUI.hpp>

// Forward decls
class CPlayer;

extern "C" {
#pragma clang attribute push(__attribute__((section(".boot"))), apply_to = function)
__attribute__((visibility("default"))) extern void _prolog();
[[maybe_unused]] __attribute__((visibility("default"))) extern void _earlyboot_memset(void *dst, char val, u32 size);
#pragma clang attribute pop

void *memcpy(void *dest, const void *src, size_t count);
}

u32 safeBlocks[] = {
    0x8056A7E4,
    0x1610,
    //    0x8056D5E0, 0x138,
    //    0x8056F654, 0x104,
    //    0x8056F874, 0x20,
    0x8056F8CC,
    0x1080,
    //    0x80571ABC, 0xD4,
    //    0x80571CCC, 0x1A0,
    //    0x80572030, 0xC,
    //    0x80572054, 0xC,
    //    0x80572088, 0x50,
    //    0x805723EC, 0xC,
    //    0x80572414, 0x100,
    //    0x8057267C, 0xC,
    0x80577BAC,
    0x14000,
    //    0x8059FBB8, 0xC,
    //    0x8059FBE8, 0x90,
    0x805A02F8,
    0x2868,
    //    0x805A53D4, 0xC,
    // 0x805A56E4,
    // 0x78C,
    //    0x805A66AC, 0x48,
    //    0x805A676C, 0x18,
    //    0x805A67EC, 0x10,
    //    0x805A6B90, 0x10,
};

void memset_start_end(u32 dst, u32 end, char val) {
  if (end > dst) return;
  u32 size = end - dst;
  memset((void *)dst, val, size);
}

[[maybe_unused]] void _earlyboot_memset(void *dst, char val, u32 size) {
  u32 start = (u32)dst;
  u32 end = start + size;
  for (u32 i = 0; i < sizeof(safeBlocks) / sizeof(u32); i += 2) {
    if (end >= start) break; // we're done

    u32 blockStart = safeBlocks[i];
    u32 blockSize = safeBlocks[i + 1];
    u32 blockEnd = blockStart + blockSize;

    // if the start is after the end, continue
    if (start > blockEnd) {
      continue;
    } else if (end < blockStart) {
      // if the end is before this block starts, then it's safe.
      // finish the memset
      memset_start_end(start, end, val);
      start = end; // we're done
      break;
    } else if (end < blockEnd) {
      // if the end address is less than our end, finish
      start = end; // we're done
      break;
    } else {
      // otherwise, write until start of block and resume after
      memset_start_end(start, blockStart, val);
      start = blockEnd;
    }
  }
  // whatever is left is after us
  memset_start_end(start, end, val);
}

struct ForceStaticInit {
  ForceStaticInit() {
    OSReport("Static initializer forced to run\n");
  }
} g_force_static_init;

void InstallHooks();
bool initialized{false};

// clang-format off
DECLARE_FUNCTION_REPLACEMENT(Hook_OSResetSystem) {
  static void Callback(int reset,u32 resetCode,int forceMenu) {
    OSReport("Hooked OSResetSystem: reset=%d code=%u forceMenu=%d\n", reset, resetCode, forceMenu);
    // initialized = false;
    // ReplaceFunctionHookPool::Reset();
    Orig(reset, resetCode, forceMenu);
  }
};
// clang-format on

void runStaticInitializers() {
  // call static initializers
  // asm volatile("bl _GLOBAL__sub_I_prime_practice.cpp\n\t");
  extern void (*__init_array_start[])(void);
  extern void (*__init_array_end[])(void);

  size_t count = __init_array_end - __init_array_start;
  OSReport("Calling %u static initializer(s)\n", count);
  for (size_t i = 0; i < count; i++) {
    __init_array_start[i]();
  }
}

void _prolog() {
  // check to see if we have hooked OSSystemReset
  u32 orig = *(u32 *)&OSResetSystem;
  if (initialized && orig == 0x7c0802a6) { // original instruction: mflr r0
    OSReport("Hooks have reset, reinitializing\n");
    initialized = false;
  }
  if (initialized) {
    DebugLog("Already called prolog once\n");
    return;
  }
  initialized = true;
  // this is the only thing in the function we replace
  asm volatile("mtfsb0 5");
  runStaticInitializers();
  PracticeMod::ClearInstance();
  Hook_OSResetSystem::InstallAtFuncPtr(&OSResetSystem);
  InstallHooks();
}

// void resetLayerStates(const CStateManager &manager) {
//  CMemoryCardSys *memorySystem = *(CMemoryCardSys **) 0x805A8C44;
//  CGameState *gameState = gpGameState;
//  uint32 currentMlvl = gameState->MLVL();
//
//  CSaveWorldIntermediate *worldIntermediates = memorySystem->worldIntermediate;
//  CSaveWorldIntermediate *intermediate = 0;
//  for (int i = 0; i < 8; i++) {
//    if (worldIntermediates[i].mlvlID == currentMlvl) {
//      intermediate = &(worldIntermediates[i]);
//      break;
//    }
//  }
//  if (intermediate != 0) {
////    intermediate = *((CSaveWorldIntermediate**)((int)memorySystem & 0x7FFFFFFF));
////    crashVar = *((int*)((int)(intermediate->mlvlID) & 0x7FFFFFFF));
////    crashVar = *((int*)((int)&(intermediate->defaultLayerStates) & 0x7FFFFFFF));
//  } else {
//    crashVar = *((int *) 0xDEAD0001);
//  }
//
//  CWorldState &worldState = gameState->StateForWorld(currentMlvl);
//  CWorldLayerState *layerState = *worldState.layerState;
//
//  rstl::vector<CWorldLayers::Area> &srcLayers = intermediate->defaultLayerStates;
//  rstl::vector<CWorldLayers::Area> &destLayers = layerState->areaLayers;
//
//  if (srcLayers.len == destLayers.len) {
//    for (int i = 0; i < srcLayers.len; i++) {
//      destLayers.ptr[i].m_layerBits = srcLayers.ptr[i].m_layerBits;
//    }
//  } else {
//    crashVar = *((int *) 0xDEAD0002);
//  }
//}
