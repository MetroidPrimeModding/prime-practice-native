#include "PracticeMod.hpp"
#include "UI/BombJumping.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CGameState.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CMFGame.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CMemory.hpp"
#include "prime/CPauseScreen.hpp"
#include "prime/CSfxManager.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CTweaks.hpp"
#include "prime/CWorldState.hpp"
#include "types.h"
#include "utils/Hook.hpp"

#include <PrimeAPI.h>
#include <os.h>
#include <prime/CFontEndUI.hpp>
#include <prime/CPlayerGun.hpp>
#include <prime/CRandom16.hpp>
#include <settings.hpp>
#include <world/WorldRenderer.hpp>

// Forward decls
class CPlayer;

extern "C" {
#pragma clang attribute push (__attribute__((section(".boot"))), apply_to=function)
__attribute__((visibility("default"))) extern void _prolog();
[[maybe_unused]] __attribute__((visibility("default"))) extern void _earlyboot_memset(void *dst, char val, u32 size);
#pragma clang attribute pop

void *memcpy(void *dest, const void *src, size_t count);
}

u32 safeBlocks[] = {
  0x8056A7E4, 0x1610,
  //    0x8056D5E0, 0x138,
  //    0x8056F654, 0x104,
  //    0x8056F874, 0x20,
  0x8056F8CC, 0x1080,
  //    0x80571ABC, 0xD4,
  //    0x80571CCC, 0x1A0,
  //    0x80572030, 0xC,
  //    0x80572054, 0xC,
  //    0x80572088, 0x50,
  //    0x805723EC, 0xC,
  //    0x80572414, 0x100,
  //    0x8057267C, 0xC,
  0x80577BAC, 0x14000,
  //    0x8059FBB8, 0xC,
  //    0x8059FBE8, 0x90,
  0x805A02F8, 0x2868,
  //    0x805A53D4, 0xC,
  0x805A56E4, 0x78C,
  //    0x805A66AC, 0x48,
  //    0x805A676C, 0x18,
  //    0x805A67EC, 0x10,
  //    0x805A6B90, 0x10,
};

void memset_start_end(u32 dst, u32 end) {
  if (end > dst) return;
  u32 size = end - dst;
  memset((void *) dst, 0, size);
}

[[maybe_unused]] void _earlyboot_memset(void *dst, char val, u32 size) {
  u32 start = (u32) dst;
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
      memset_start_end(start, end);
      start = end; // we're done
      break;
    } else if (end < blockEnd) {
      // if the end address is less than our end, finish
      start = end; // we're done
      break;
    } else {
      // otherwise, write until start of block and resume after
      memset_start_end(start, blockStart);
      start = blockEnd;
    }
  }
  // whatever is left is after us
  memset_start_end(start, end);
}

bool initialized{false};
void _prolog() {
  if (initialized) {
    OSReport("Already called prolog once");
    return;
  }
  initialized = true;
  // call static initializers
  // TODO: maybe load this from the .elf somehow, but this works for now
  asm volatile("bl _GLOBAL__sub_I_prime_practice.cpp\n\t");
  // null out prac mod instance; this is called from reset()
  PracticeMod::instance = nullptr;
}

//CPlayerGun::DropBomb
DEFINE_HOOK(0x8003fc44, CPlayerGun_DropBomb) {
  // void DropBombHook(CPlayerGun *thiz, EBWeapon weapon, CStateManager &mgr) {
  // CPlayerGun *thiz = regs->GetCallArg<CPlayerGun*>(0);
  auto weapon = regs->getCallArg<EBWeapon>(1);
  // CStateManager *mgr = regs->GetCallArg<CStateManager*>(2);
  if (weapon == EBWeapon::Bomb) {
    GUI::bombDropped();
  }
}

bool tweaksPatched = false;
void TweakPatcher() {
  if (tweaksPatched) return;
  if (!g_TweakGame) return;
  *g_TweakAutoMappers->maxCamDist() *= 2;
  tweaksPatched = true;
}

// CGraphics::EndScene
DEFINE_HOOK(0x8030BAC0, CGraphics_EndScene) {
  TweakPatcher();
  if (!PracticeMod::instance) {
    PracticeMod::instance = new PracticeMod();
  }
  PracticeMod::instance->render();
}

// CPauseScreen::ProcessControllerInput
DEFINE_HOOK(0x80072BB4, CPauseScreen_ProcessControllerInput) {
  auto *self = regs->getCallArg<CPauseScreen *>(0);
  auto *mgr = regs->getCallArg<CStateManager *>(1);
  auto *input = regs->getCallArg<CFinalInput *>(2);

  if (!self->IsLoaded()) { return; }
  if (self->x8_curSubscreen == CPauseScreen::ESubScreen_ToGame) { return; }

  if (self->InputEnabled()) {
    PracticeMod::instance->pauseScreenOpened();
    if (input->PStart()) {
      PracticeMod::instance->pauseScreenClosed();

      //Play some noises too
      CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
      self->StartTransition(0.5f, *mgr, CPauseScreen::ESubScreen_ToGame, 2);
    } else if (input->PZ()) {
      PracticeMod::instance->menuActive = !PracticeMod::instance->menuActive;
    }
  }
  if (PracticeMod::instance->menuActive) {
    regs->earlyReturn = true;
  }
}

// CMainFlow::OnMessage
DEFINE_HOOK(0x80023908, CMainFlow_OnMessage) {
  // auto *self = regs->GetThis<CMainFlow *>();
  auto *msg = regs->getCallArg<CArchitectureMessage *>(1);
  // auto *queue = regs->GetCallArg<CArchitectureQueue *>(2);

  if (msg->x4_type == EArchMsgType_UserInput) {
    if (!PracticeMod::instance) {
      PracticeMod::instance = new PracticeMod();
    }
    CArchMsgParmUserInput *status = (CArchMsgParmUserInput *) msg->x8_parm.RawPointer();
    // The mod 4 is just for safety
    PracticeMod::instance->inputs[status->x4_parm.ControllerIdx() % 4] = status->x4_parm;
    if (status->x4_parm.ControllerIdx() == 0) {
      PracticeMod::instance->HandleInputs();
      // TODO: move this to hook in Game::Update or something
      PracticeMod::instance->update(0);
    }
  }
}

//CStateManager::DrawDebugStuff
DEFINE_HOOK(0x80045BA0, CStateManager_DrawDebugStuff) {
  // auto *mgr = regs->GetThis<CStateManager *>();
  if (!PracticeMod::instance) {
    PracticeMod::instance = new PracticeMod();
  }
  WorldRenderer::RenderWorld();
  //    NewPauseScreen::instance->Render();
}

//CStateManager::Update
DEFINE_HOOK(0x8004A7CC, CStateManager_Update) {
  // auto *self = regs->getThis<CStateManager *>();
  auto dt = regs->getCallArg<float>(1);
  if (PracticeMod::instance) {
    PracticeMod::instance->update(dt);
  }
}

//CScriptSpecialFunction::ShouldSkipCinematic
DEFINE_HOOK(0x80151868, CScriptSpecialFunction_ShouldSkipCinematic) {
  regs->earlyReturn = true;
  regs->setReturnValue(true);
}

//CMapWorldInfo::IsMapped
DEFINE_HOOK(0x80168324, CMapWorldInfo__IsMapped) {
  regs->earlyReturn = true;
  regs->setReturnValue(true);
}

//CMapWorldInfo::IsWorldVisible
DEFINE_HOOK(0x80168440, CMapWorldInfo_IsWorldVisible) {
  regs->earlyReturn = true;
  regs->setReturnValue(true);
}

//CMapWorldInfo::IsAreaVisible
DEFINE_HOOK(0x80167D04, CMapWorldInfo__IsAreaVisible) {
  regs->earlyReturn = true;
  regs->setReturnValue(true);
}

//CMapArea::GetIsVisibleToAutoMapper
DEFINE_HOOK(0x80080D2C, CMapArea_GetIsVisibleToAutoMapper) {
  regs->earlyReturn = true;
  regs->setReturnValue(true);
}

//CAutoMapper::Draw
DEFINE_HOOK(0x80099004, CAutoMapper__Draw) {
  auto *self = regs->getThis<CAutoMapper *>();
  EAutoMapperState state = self->state();
  EAutoMapperState nextState = self->nextState();
  PracticeMod::instance->mapActive = state == EAutoMapperState::MapScreen && nextState ==
                                        EAutoMapperState::MapScreen;
}

//CAutoMapper::ProcessMapScreenInput
DEFINE_HOOK(0x8009AC7C, CAutoMapper_ProcessMapScreenInput) {
  auto *self = regs->getThis<CAutoMapper *>();
  auto *input = regs->getCallArg<CFinalInput *>(1);
  EAutoMapperState state = self->state();
  EAutoMapperState nextState = self->nextState();
  if (state == EAutoMapperState::MapScreen && nextState == EAutoMapperState::MapScreen) {
    if (input->PX()) {
      IWorld *world = self->world();
      u32 worldid = world->IGetWorldAssetId();
      TAreaId areaId = *self->curAreaId();
      IGameArea *area = world->IGetAreaAlways(areaId);
      u32 mrea = area->IGetAreaAssetId();
      //    OSReport("world %x %x %d vt %x\n", world, area, areaId, GetVtable(world));
      warp(worldid, mrea);
    }
  }
}

#ifdef DEBUG
//CMainFlow::AdvanceGameState
DEFINE_HOOK(0x80023854, CMainFlow_AdvanceGameState) {
  auto *self = regs->getThis<CMainFlow *>();
  auto queue = regs->getCallArg<CArchitectureQueue *>(1);
  OSReport("Hooked advance game state\n");
  // This hook is only compiled in if mode == Debug

  // Hook into CMainFlow::AdvanceGameState(). When this function is called with
  // the game state set to PreFrontEnd, that indicates that engine initialization
  // is complete and the game is proceeding to the main menu. We hook in here to
  // bypass the main menu and boot directly into the game.
  static bool sHasDoneInitialBoot = false;

  // Make sure the patch does not run twice if the player quits out to main menu
  if (!sHasDoneInitialBoot && self->GetGameState() == 7) {
    sHasDoneInitialBoot = true;
    CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
    gameState->SetCurrentWorldId(0x83F6FF6F); // chozo ruins
    gameState->CurrentWorldState().SetDesiredAreaAssetId(0x47E73BC5); // gathering hall
    self->SetGameState(kCFS_Game, *queue);
    regs->earlyReturn = true;
  }
}
#endif

// CRandom16::Next
DEFINE_HOOK(0x80312520, CRandom16_Next) {
  CRandom16 *self = regs->getThis<CRandom16 *>();
  if (!SETTINGS.RNG_lockSeed) {
    self->m_seed = (self->m_seed * 0x41c64e6d) + 0x00003039;
  }
  regs->setReturnValue((self->m_seed >> 16) & 0xffff);
  regs->earlyReturn = true;
}

//void resetLayerStates(const CStateManager &manager) {
//  CMemoryCardSys *memorySystem = *(CMemoryCardSys **) 0x805A8C44;
//  CGameState *gameState = *(CGameState **) (0x80457798 + 0x134);
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
