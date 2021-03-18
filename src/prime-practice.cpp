#include <PrimeAPI.h>
#include <prime/CFontEndUI.hpp>
#include <os.h>

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
#include "TextRenderer.hpp"
#include "NewPauseScreen.hpp"

// Forward decls
class CPlayer;

extern "C" {
void _prolog();
}

bool UpdateHealth(CPlayer *, CStateManager &);
void VISetNextFrameBuffer(void *fb);
void debug();
void RenderHook();
void PauseScreenDrawReplacement(CPauseScreen *);
void PauseControllerInputHandler(CPauseScreen *pause, CStateManager &mgr, const CFinalInput &input);
void resetLayerStates(const CStateManager &manager);
CIOWin::EMessageReturn IOWinMessageHook(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue);
void drawDebugStuff(CStateManager *);
CFrontEndUI *CFrontEndConstructorPatch(CFrontEndUI *thiz, CArchitectureQueue &queue);

int crashVar;

extern "C" {
void *memcpy(void *dest, const void *src, size_t count);
}

void operator delete(void *ptr) {
  CMemory::Free(ptr);
}

// Impls
extern "C" {
extern int _INIT_START;
__attribute__((visibility("default"))) extern void __rel_prolog();
}

__attribute__((visibility("default"))) void __rel_prolog() {
  _prolog();
}

void _prolog() {
  MODULE_INIT;
  char buffer[32];
  sprintf(buffer, "_INIT_START= %8x\n", (int) (&_INIT_START));
  OSReport(buffer);
  sprintf(buffer, "_prolog= %8x\n", (int) (&_prolog));
  OSReport(buffer);
}

void PauseScreenDrawReplacement(CPauseScreen *pause) {
  if (!pause->IsLoaded()) { return; }
}

void PauseControllerInputHandler(CPauseScreen *pause, CStateManager &mgr, const CFinalInput &input) {
  if (!pause->IsLoaded()) { return; }
  if (pause->x8_curSubscreen == CPauseScreen::ESubScreen_ToGame) { return; }

  if (pause->InputEnabled()) {
    // Only close if you aren't holding the reload hotkey
    if (input.PStart() && !(input.DL() || input.DR())) {
      NewPauseScreen::instance->hide();

      //Play some noises too
      CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId);
      pause->StartTransition(0.5f, mgr, CPauseScreen::ESubScreen_ToGame, 2);
    } else if (NewPauseScreen::instance->frames < 0) {
      NewPauseScreen::instance->show();
    }
  }
}

void resetLayerStates(const CStateManager &manager) {
  CMemoryCardSys *memorySystem = *(CMemoryCardSys **) 0x805A8C44;
  CGameState *gameState = *(CGameState **) (0x80457798 + 0x134);
  uint32 currentMlvl = gameState->MLVL();

  CSaveWorldIntermediate *worldIntermediates = memorySystem->worldIntermediate;
  CSaveWorldIntermediate *intermediate = 0;
  for (int i = 0; i < 8; i++) {
    if (worldIntermediates[i].mlvlID == currentMlvl) {
      intermediate = &(worldIntermediates[i]);
      break;
    }
  }
  if (intermediate != 0) {
//    intermediate = *((CSaveWorldIntermediate**)((int)memorySystem & 0x7FFFFFFF));
//    crashVar = *((int*)((int)(intermediate->mlvlID) & 0x7FFFFFFF));
//    crashVar = *((int*)((int)&(intermediate->defaultLayerStates) & 0x7FFFFFFF));
  } else {
    crashVar = *((int *) 0xDEAD0001);
  }

  CWorldState &worldState = gameState->StateForWorld(currentMlvl);
  CWorldLayerState *layerState = *worldState.layerState;

  rstl::vector<CWorldLayers::Area> &srcLayers = intermediate->defaultLayerStates;
  rstl::vector<CWorldLayers::Area> &destLayers = layerState->areaLayers;

  if (srcLayers.len == destLayers.len) {
    for (int i = 0; i < srcLayers.len; i++) {
      destLayers.ptr[i].m_layerBits = srcLayers.ptr[i].m_layerBits;
    }
  } else {
    crashVar = *((int *) 0xDEAD0002);
  }
}

void RenderHook() {
//  CGraphics::BeginScene();
  if (!NewPauseScreen::instance) {
    NewPauseScreen::instance = new NewPauseScreen();
  }
  if (NewPauseScreen::instance->shouldRenderGloballyInsteadOfInWorld()) {
    NewPauseScreen::instance->Render();
  }
  CGraphics::EndScene();
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
    }
  }

  return thiz->OnMessage(msg, queue);
}

void drawDebugStuff(CStateManager *mgr) {
  if (!NewPauseScreen::instance) {
    NewPauseScreen::instance = new NewPauseScreen();
  }
  NewPauseScreen::instance->RenderWorld();
  if (!NewPauseScreen::instance->shouldRenderGloballyInsteadOfInWorld()) {
    NewPauseScreen::instance->Render();
  }
}
//
//CFrontEndUI *CFrontEndConstructorPatch(CFrontEndUI *thiz, CArchitectureQueue &queue) {
//    thiz = new ((void*)thiz) CFrontEndUI(queue);
//    *thiz->getPhase() = 6;
//  //CStateManager *mgr = ((CStateManager *) 0x8045A1A8);
////    mgr->GetWorld()->SetPauseState(true);
//
//  CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
//  gameState->SetCurrentWorldId(0x83F6FF6F);
////  gameState->CurrentWorldState().SetDesiredAreaAssetId(0x44E528F6);
//
//  return thiz;
//}

// Hooks
void Hook_CMainFlow_AdvanceGameState(CMainFlow *pMainFlow, CArchitectureQueue &Queue) {
  // Hook into CMainFlow::AdvanceGameState(). When this function is called with
  // the game state set to PreFrontEnd, that indicates that engine initialization
  // is complete and the game is proceeding to the main menu. We hook in here to
  // bypass the main menu and boot directly into the game.
  static bool sHasDoneInitialBoot = false;

  // Make sure the patch does not run twice if the player quits out to main menu
  if (!sHasDoneInitialBoot && pMainFlow->GetGameState() == 7) {
    sHasDoneInitialBoot = true;
    CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
    gameState->SetCurrentWorldId(0x39F2DE28);
    gameState->CurrentWorldState().SetDesiredAreaAssetId(0xC44E7A07);
    pMainFlow->SetGameState(kCFS_Game, Queue);
    return;
  } else {
    pMainFlow->AdvanceGameState(Queue);
  }
}
