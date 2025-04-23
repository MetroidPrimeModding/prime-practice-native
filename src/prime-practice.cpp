#include <PrimeAPI.h>
#include <prime/CFontEndUI.hpp>
#include <os.h>
#include <settings.hpp>
#include <world/WorldRenderer.hpp>
#include <prime/CPlayerGun.hpp>
#include <prime/CRandom16.hpp>
#include "UI/BombJumping.hpp"
#include "types.h"
#include "prime/CMain.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CGameState.hpp"
#include "prime/CWorldState.hpp"
#include "prime/CHealthInfo.hpp"
#include "prime/CMemory.hpp"
#include "prime/CSimplePool.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CPauseScreen.hpp"
#include "prime/CSfxManager.hpp"
#include "prime/CColor.hpp"
#include "prime/CTextGui.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CMemoryCardSys.hpp"
#include "prime/CMFGame.hpp"
#include "prime/CIOWinManager.hpp"
#include "prime/CMainFlow.hpp"
#include "NewPauseScreen.hpp"
#include "prime/CTweaks.hpp"

// Forward decls
class CPlayer;

void debug();
void RenderHook();
void PauseScreenDrawReplacement(CPauseScreen *);
void CStateManager_UpdateHook(CStateManager *self, float dt);
void PauseControllerInputHandler(CPauseScreen *pause, CStateManager &mgr, const CFinalInput &input);
CIOWin::EMessageReturn IOWinMessageHook(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue);
void drawDebugStuff(CStateManager *);
CFrontEndUI *CFrontEndConstructorPatch(CFrontEndUI *thiz, CArchitectureQueue &queue);
void DropBombHook(CPlayerGun *thiz, EBWeapon weapon, CStateManager &mgr);
bool SkipCutsceneHook(void *, void *);

// Automapper hooks
//bool IsDoorVisited(void *, u32);
//bool IsAreaVisited(void *, u32);
bool IsMapped(void *, u32);
bool IsWorldVisible(void *, u32);
bool IsAreaVisible(void *, u32);
bool IsAnythingSet(void *);
bool GetIsVisibleToAutoMapper(void *, bool, bool);
void MapScreenInputHook(CAutoMapper *mapper, const CFinalInput &input, CStateManager &mgr);
void MapScreenDrawHook(CAutoMapper *mapper, const CStateManager &mgr, const CTransform4f &xf, float alpha);
#ifdef DEBUG
void Hook_CMainFlow_AdvanceGameState(CMainFlow *pMainFlow, CArchitectureQueue &Queue);
#endif

int Hook_CRandom16Next(CRandom16 *rng);

extern "C" {
#pragma clang attribute push (__attribute__((section(".boot"))), apply_to=function)

__attribute__((visibility("default"))) extern void _prolog();
[[maybe_unused]] __attribute__((visibility("default"))) extern void _earlyboot_memset(void *dst, char val, u32 size);
#pragma clang attribute pop

void *memcpy(void *dest, const void *src, size_t count);
}

void operator delete(void *ptr) {
  CMemory::Free(ptr);
}

void ApplyCodePatches();

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

void _prolog() {
  // null out prac mod instance; this is called from reset()
  NewPauseScreen::instance = nullptr;
  ApplyCodePatches();
  char buffer[32];
  sprintf(buffer, "_prolog= %8x\n", (int) (&_prolog));
  OSReport(buffer);
}

void Relocate_Addr32(void *pRelocAddress, void *pSymbolAddress) {
  uint32 *pReloc = (uint32 *) pRelocAddress;
  *pReloc = (uint32) pSymbolAddress;
}

void Relocate_Rel24(void *pRelocAddress, void *pSymbolAddress) {
  uint32 *pReloc = (uint32 *) pRelocAddress;
  uint32 instruction = *pReloc;
  uint32 AA = (instruction >> 1) & 0x1;
  *pReloc = (instruction & ~0x3FFFFFC) |
            (AA == 0 ? ((uint32) pSymbolAddress - (uint32) pRelocAddress) : (uint32) pSymbolAddress);
}

template<typename FuncType>
void ReplaceFunction(FuncType original, void *replacement) {
  // void ReplaceFunction(void* original, void* replacement) {

  union {
    FuncType mfp;
    void *addr;
  } u;
  u.mfp = original;

  uint32_t *originalFunctionPtr = (uint32_t *) u.addr;
  uint32_t *replacementFunctionPtr = (uint32_t *) replacement;
  int32_t diff = (int32_t) replacementFunctionPtr - (int32_t) originalFunctionPtr;
  // Make sure the offset fits in 24 bits (word aligned, signed)
  int32_t li = (diff >> 2) & 0x00FFFFFF;
  // Handle negative offsets (sign extension for PPC branch)
  if (diff < 0) {
    li |= 0x01000000; // Add sign bit for 24-bit
  }
  uint32_t instruction = (18 << 26) | (li << 2);
  // aa = 0 (relative), lk = 0 (no link)
  *originalFunctionPtr = instruction;
}

void ApplyCodePatches() {
  // CGraphics::EndScene
  Relocate_Rel24((void *) 0x80005734, reinterpret_cast<void *>(&RenderHook));
  Relocate_Rel24((void *) 0x800061F4, reinterpret_cast<void *>(&RenderHook));
  Relocate_Rel24((void *) 0x802BDC5C, reinterpret_cast<void *>(&RenderHook));
  // CPauseScreen::Draw
  Relocate_Rel24((void *) 0x80108DB4, reinterpret_cast<void *>(&PauseScreenDrawReplacement));
  // CPauseScreen::ProcessControllerInput
  Relocate_Rel24((void *) 0x80107A28, reinterpret_cast<void *>(&PauseControllerInputHandler));
  // CMainFlow::OnMessage
  Relocate_Addr32((void *) 0x803D9934, reinterpret_cast<void *>(&IOWinMessageHook));
  //CStateManager::DrawDebugStuff
  Relocate_Rel24((void *) 0x80046B88, reinterpret_cast<void *>(&drawDebugStuff));
  //CPlayerGun::DropBomb
  Relocate_Rel24((void *) 0x8003dc74, reinterpret_cast<void *>(&DropBombHook));
  Relocate_Rel24((void *) 0x8003dd40, reinterpret_cast<void *>(&DropBombHook));
  Relocate_Rel24((void *) 0x80152510, reinterpret_cast<void *>(&DropBombHook));
  Relocate_Rel24((void *) 0x80152528, reinterpret_cast<void *>(&DropBombHook));
  //CStateManager::Update
  Relocate_Rel24((void *) 0x80024854, reinterpret_cast<void *>(&CStateManager_UpdateHook));
  Relocate_Rel24((void *) 0x80024a0c, reinterpret_cast<void *>(&CStateManager_UpdateHook));
  //CScriptSpecialFunction::ShouldSkipCinematic
  Relocate_Rel24((void *) 0x80044a04, reinterpret_cast<void *>(&SkipCutsceneHook));
  Relocate_Rel24((void *) 0x801521d4, reinterpret_cast<void *>(&SkipCutsceneHook));
  //CMapWorldInfo::IsDoorVisited
  //  Relocate_Rel24((void *) 0x80168d24, reinterpret_cast<void *>(&IsDoorVisited));
  //  Relocate_Rel24((void *) 0x800e9274, reinterpret_cast<void *>(&IsDoorVisited));
  //  Relocate_Rel24((void *) 0x800e8b10, reinterpret_cast<void *>(&IsDoorVisited));
  //CMapWorldInfo::IsAreaVisited
  //  Relocate_Rel24((void *) 0x80167d34, reinterpret_cast<void *>(&IsAreaVisited));
  //  Relocate_Rel24((void *) 0x8009fd1c, reinterpret_cast<void *>(&IsAreaVisited));
  //  Relocate_Rel24((void *) 0x80098a28, reinterpret_cast<void *>(&IsAreaVisited));
  //  Relocate_Rel24((void *) 0x8004c134, reinterpret_cast<void *>(&IsAreaVisited));
  //CMapWorldInfo:IsMapped
  Relocate_Rel24((void *) 0x8016846c, reinterpret_cast<void *>(&IsMapped));
  Relocate_Rel24((void *) 0x80167d50, reinterpret_cast<void *>(&IsMapped));
  Relocate_Rel24((void *) 0x80098a08, reinterpret_cast<void *>(&IsMapped));
  //CMapWorldInfo::IsWorldVisible
  Relocate_Rel24((void *) 0x800a0ea0, reinterpret_cast<void *>(&IsWorldVisible));
  Relocate_Rel24((void *) 0x800a0088, reinterpret_cast<void *>(&IsWorldVisible));
  Relocate_Rel24((void *) 0x8009fc6c, reinterpret_cast<void *>(&IsWorldVisible));
  Relocate_Rel24((void *) 0x8009f90c, reinterpret_cast<void *>(&IsWorldVisible));
  Relocate_Rel24((void *) 0x80095f14, reinterpret_cast<void *>(&IsWorldVisible));
  //CMapWorldInfo::IsAreaVisible
  Relocate_Rel24((void *) 0x800e8a80, reinterpret_cast<void *>(&IsAreaVisible));
  Relocate_Rel24((void *) 0x800a0e90, reinterpret_cast<void *>(&IsAreaVisible));
  Relocate_Rel24((void *) 0x800a00d8, reinterpret_cast<void *>(&IsAreaVisible));
  Relocate_Rel24((void *) 0x8009fc5c, reinterpret_cast<void *>(&IsAreaVisible));
  Relocate_Rel24((void *) 0x8009f8fc, reinterpret_cast<void *>(&IsAreaVisible));
  Relocate_Rel24((void *) 0x80095f04, reinterpret_cast<void *>(&IsAreaVisible));
  //CMapWorldInfo::IsAnythingSet
  Relocate_Rel24((void *) 0x80096414, reinterpret_cast<void *>(&IsAnythingSet));
  Relocate_Rel24((void *) 0x80202448, reinterpret_cast<void *>(&IsAnythingSet));
  //CAutoMapper::Draw
  Relocate_Rel24((void *) 0x80108d1c, reinterpret_cast<void *>(&MapScreenDrawHook));
  //CAutoMapper::ProcessMapScreenInput
  Relocate_Rel24((void *) 0x8009af68, reinterpret_cast<void *>(&MapScreenInputHook));
  //CMapArea::GetIsVisibleToAutoMapper
  Relocate_Rel24((void *) 0x80095f24, reinterpret_cast<void *>(&GetIsVisibleToAutoMapper));
  Relocate_Rel24((void *) 0x8009f91c, reinterpret_cast<void *>(&GetIsVisibleToAutoMapper));
  Relocate_Rel24((void *) 0x8009fc7c, reinterpret_cast<void *>(&GetIsVisibleToAutoMapper));
  Relocate_Rel24((void *) 0x800a0eb0, reinterpret_cast<void *>(&GetIsVisibleToAutoMapper));

#ifdef DEBUG
  //CMainFlow::AdvanceGameState
  Relocate_Rel24((void *) 0x80023950, reinterpret_cast<void *>(&Hook_CMainFlow_AdvanceGameState));
#endif

  // crandom16 replace the whole function
  ReplaceFunction(&CRandom16::Next, (void *) &Hook_CRandom16Next);
}

bool tweaksPatched = false;

void TweakPatcher() {
  if (tweaksPatched) return;
  if (!g_TweakGame) return;
  *g_TweakAutoMappers->maxCamDist() *= 2;
  tweaksPatched = true;
}

void PauseScreenDrawReplacement(CPauseScreen *pause) {
  if (!pause->IsLoaded()) { return; }
  pause->Draw();
}

void PauseControllerInputHandler(CPauseScreen *pause, CStateManager &mgr, const CFinalInput &input) {
  if (!pause->IsLoaded()) { return; }
  if (pause->x8_curSubscreen == CPauseScreen::ESubScreen_ToGame) { return; }

  if (pause->InputEnabled()) {
    NewPauseScreen::instance->show();
    if (input.PStart()) {
      NewPauseScreen::instance->hide();

      //Play some noises too
      CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
      pause->StartTransition(0.5f, mgr, CPauseScreen::ESubScreen_ToGame, 2);
    } else if (input.PZ()) {
      NewPauseScreen::instance->menuActive = !NewPauseScreen::instance->menuActive;
    }
  }
  if (!NewPauseScreen::instance->menuActive) {
    pause->ProcessControllerInput(mgr, input);
  }
}

void RenderHook() {
  TweakPatcher();
  //  CGraphics::BeginScene();
  if (!NewPauseScreen::instance) {
    NewPauseScreen::instance = new NewPauseScreen();
  }
  NewPauseScreen::instance->Render();
  CGraphics::EndScene();
}

void CStateManager_UpdateHook(CStateManager *self, float dt) {
  if (NewPauseScreen::instance) {
    NewPauseScreen::instance->update(dt);
  }
  self->Update(dt);
}

CIOWin::EMessageReturn IOWinMessageHook(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue) {
  if (msg.x4_type == EArchMsgType_UserInput) {
    if (!NewPauseScreen::instance) {
      NewPauseScreen::instance = new NewPauseScreen();
    }
    CArchMsgParmUserInput *status = (CArchMsgParmUserInput *) msg.x8_parm.RawPointer();
    // The mod 4 is just for safety
    NewPauseScreen::instance->inputs[status->x4_parm.ControllerIdx() % 4] = status->x4_parm;
    if (status->x4_parm.ControllerIdx() == 0) {
      NewPauseScreen::instance->HandleInputs();
      // TODO: move this to hook in Game::Update or something
      NewPauseScreen::instance->update(0);
    }
  }

  return thiz->OnMessage(msg, queue);
}

void drawDebugStuff(CStateManager *mgr) {
  if (!NewPauseScreen::instance) {
    NewPauseScreen::instance = new NewPauseScreen();
  }
  WorldRenderer::RenderWorld();
  //    NewPauseScreen::instance->Render();
}

void DropBombHook(CPlayerGun *thiz, EBWeapon weapon, CStateManager &mgr) {
  if (weapon == EBWeapon::Bomb) {
    GUI::bombDropped();
  }
  thiz->DropBomb(weapon, mgr);
}

bool SkipCutsceneHook(void *, void *) {
  return true;
}

//bool IsDoorVisited(void *, u32) {
//  return true;
//}
//
//bool IsAreaVisited(void *, u32) {
//  return true;
//}

bool IsMapped(void *, u32) {
  return true;
}

bool IsWorldVisible(void *, u32) {
  return true;
}

bool IsAreaVisible(void *, u32) {
  return true;
}

bool IsAnythingSet(void *) {
  return true;
}

bool GetIsVisibleToAutoMapper(void *, bool, bool) {
  return true;
}

void MapScreenDrawHook(CAutoMapper *mapper, const CStateManager &mgr, const CTransform4f &xf, float alpha) {
  mapper->Draw(mgr, xf, alpha);
  EAutoMapperState state = mapper->state();
  EAutoMapperState nextState = mapper->nextState();
  NewPauseScreen::instance->mapActive = state == EAutoMapperState::MapScreen && nextState ==
                                        EAutoMapperState::MapScreen;
}

void MapScreenInputHook(CAutoMapper *mapper, const CFinalInput &input, CStateManager &mgr) {
  mapper->ProcessMapScreenInput(input, mgr);
  EAutoMapperState state = mapper->state();
  EAutoMapperState nextState = mapper->nextState();
  if (state == EAutoMapperState::MapScreen && nextState == EAutoMapperState::MapScreen) {
    if (input.PX()) {
      IWorld *world = mapper->world();
      u32 worldid = world->IGetWorldAssetId();
      TAreaId areaId = *mapper->curAreaId();
      IGameArea *area = world->IGetAreaAlways(areaId);
      u32 mrea = area->IGetAreaAssetId();
      //    OSReport("world %x %x %d vt %x\n", world, area, areaId, GetVtable(world));
      warp(worldid, mrea);
    }
  }
}

#ifdef DEBUG

void Hook_CMainFlow_AdvanceGameState(CMainFlow *pMainFlow, CArchitectureQueue &Queue) {
  OSReport("Hooked advance game state\n");
  // This hook is only compiled in if mode == Debug

  // Hook into CMainFlow::AdvanceGameState(). When this function is called with
  // the game state set to PreFrontEnd, that indicates that engine initialization
  // is complete and the game is proceeding to the main menu. We hook in here to
  // bypass the main menu and boot directly into the game.
  static bool sHasDoneInitialBoot = false;

  // Make sure the patch does not run twice if the player quits out to main menu
  if (!sHasDoneInitialBoot && pMainFlow->GetGameState() == 7) {
    sHasDoneInitialBoot = true;
    CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
    gameState->SetCurrentWorldId(0x83F6FF6F); // chozo ruins
    gameState->CurrentWorldState().SetDesiredAreaAssetId(0x47E73BC5); // gathering hall
    pMainFlow->SetGameState(kCFS_Game, Queue);
    return;
  } else {
    pMainFlow->AdvanceGameState(Queue);
  }
}

#endif

int Hook_CRandom16Next(CRandom16 *rng) {
  if (!SETTINGS.RNG_lockSeed) {
    rng->m_seed = (rng->m_seed * 0x41c64e6d) + 0x00003039;
  }
  return (rng->m_seed >> 16) & 0xffff;

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
