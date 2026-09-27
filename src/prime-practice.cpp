#include "PracticeMod.hpp"
#include "carveouts.h"
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

void memset_start_end(u32 dst, u32 end, char val) {
  if (end < dst) return;
  u32 size = end - dst;
  memset((void *)dst, val, size);
}

[[maybe_unused]] void _earlyboot_memset(void *dst, char val, u32 size) {
  u32 start = (u32)dst;
  u32 end = start + size;
  // CARVEOUT_SAFE_BLOCKS is sorted by address
  for (u32 i = 0; i < sizeof(CARVEOUT_SAFE_BLOCKS) / sizeof(u32) && start < end; i += 2) {
    u32 blockStart = CARVEOUT_SAFE_BLOCKS[i];
    u32 blockEnd = blockStart + CARVEOUT_SAFE_BLOCKS[i + 1];
    if (blockEnd <= start) continue;
    if (blockStart >= end) break;
    memset_start_end(start, blockStart, val); // no-op if start is already inside the block
    start = blockEnd;
  }
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

static void patchCode(u32 addr, u32 instr) {
  *(u32 *)addr = instr;
  DCFlushRange((void *)addr, 4);
  ICInvalidateRange((void *)addr, 4);
}

// Keep the game from reaching code overwritten by mod code (see "stomps" in carveouts.json). Must run before
// CDolphinController::Initialize, so this can't wait for the PracticeMod constructor.
static void disableStompedFeatures() {
  // GBA SDK
  patchCode(0x8034F694, 0x60000000); // CDolphinController::Initialize: bl GBAInit -> nop

  // Extras menu (SFusionBonusFrame), which was the only way into the GBA link and NES Metroid. Selecting it does
  // nothing, so kS_FusionBonus is never entered, and CFrontEndUI never creates the frame.
  patchCode(0x8002133C, 0x60000000); // SFrontEndFrame::DoAdvance: stw (mAction = kEA_FusionBonus) -> nop
  patchCode(0x80020168, 0x60000000); // SNewFileSelectFrame::DoFileselectAdvance: stw (kA_FusionBonus) -> nop
  patchCode(0x8001CEBC, 0x38600000); // CFrontEndUI::Update: bl operator new (SFusionBonusFrame) -> li r3, 0
  patchCode(0x8001CED8, 0x60000000); // CFrontEndUI::Update: bl ~SFusionBonusFrame -> nop
  patchCode(0x8001D0D8, 0x38600001); // CFrontEndUI::Update: bl SFusionBonusFrame::PumpLoad -> li r3, 1
  // Only reached in kS_FusionBonus; patched anyway so a stray transition can't jump into mod code.
  patchCode(0x8001D2F8, 0x60000000); // CFrontEndUI::Update: bl SFusionBonusFrame::Update -> nop
  patchCode(0x8001CA44, 0x60000000); // CFrontEndUI::ProcessUserInput: bl SFusionBonusFrame::ProcessUserInput -> nop
  patchCode(0x8001C3D8, 0x60000000); // CFrontEndUI::Draw: bl SFusionBonusFrame::Draw -> nop
  // The destructors don't null-check before these calls; the callees check `this` themselves.
  patchCode(0x8001DA30, 0x60000000); // ~CFrontEndUI: bl ~SFusionBonusFrame -> nop
  patchCode(0x8001DA08, 0x60000000); // ~CFrontEndUI: bl ~SNesEmulatorFrame -> nop

  // Image gallery (CSlideShow). With no galleries unlocked, the menus grey the entry out and make it unselectable.
  patchCode(0x80021424, 0x38600000); // SFrontEndFrame::Update: bl SlideShowGalleryFlags -> li r3, 0
  patchCode(0x8001FBA8, 0x38600000); // SNewFileSelectFrame::ClearFrameContents: bl SlideShowGalleryFlags -> li r3, 0
  // Unreachable once the entry is unselectable; patched anyway so a stray selection can't jump into mod code.
  patchCode(0x8002135C, 0x60000000); // SFrontEndFrame::DoAdvance: stw (mAction = kEA_SlideShow) -> nop
  patchCode(0x8002017C, 0x60000000); // SNewFileSelectFrame::DoFileselectAdvance: stw (kA_SlideShow) -> nop
  patchCode(0x8001C96C, 0x60000000); // CFrontEndUI::ProcessUserInput: bl StartSlideShow -> nop
  patchCode(0x8001C9EC, 0x60000000); // CFrontEndUI::ProcessUserInput: bl StartSlideShow -> nop
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
  disableStompedFeatures();
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
