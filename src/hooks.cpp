#include "PracticeMod.hpp"
#include "UI/BombJumping.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CGameState.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CMFGame.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CMapArea.hpp"
#include "prime/CMapWorldInfo.hpp"
#include "prime/CPauseScreen.hpp"
#include "prime/CScriptSpecialFunction.hpp"
#include "prime/CSfxManager.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CTweaks.hpp"
#include "prime/CWorldState.hpp"
#include "types.h"
#include "utils/ReplaceFunction.hpp"
#include <os.h>
#include <prime/CFontEndUI.hpp>
#include <prime/CPlayerGun.hpp>
#include <prime/CRandom16.hpp>
#include <settings.hpp>
#include <world/WorldRenderer.hpp>

// clang format loves to just stomp everything here in the least useful possible way
// clang-format off

bool tweaksPatched = false;
void TweakPatcher() {
  if (tweaksPatched) return;
  if (!g_TweakGame) return;
  *g_TweakAutoMappers->maxCamDist() *= 2;
  tweaksPatched = true;
}

// CGraphics::EndScene
DECLARE_FUNCTION_REPLACEMENT(CGraphics_EndScene) {
  static void Callback() {
    TweakPatcher();
    PracticeMod::GetInstance()->render();
    Orig();
  }
};


// CPlayerGun::DropBomb
DECLARE_FUNCTION_REPLACEMENT(CPlayerGun_DropBomb) {
  static void Callback(CPlayerGun *self, EBWeapon weapon, CStateManager &mgr) {
    if (weapon == EBWeapon::Bomb) {
      GUI::bombDropped();
    }
    Orig(self, weapon, mgr);
  }
};

// CPauseScreen::ProcessControllerInput
DECLARE_FUNCTION_REPLACEMENT(CPauseScreen_ProcessControllerInput) {
  static void Callback(CPauseScreen *self, CStateManager &mgr, const CFinalInput &input) {
    if (!self->IsLoaded()) return;
    if (self->x8_curSubscreen == CPauseScreen::ESubScreen_ToGame) return;

    if (self->InputEnabled()) {
      PracticeMod::GetInstance()->pauseScreenOpened();
      if (input.PStart()) {
        PracticeMod::GetInstance()->pauseScreenClosed();

        // Play some noises too
        CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
        self->StartTransition(0.5f, mgr, CPauseScreen::ESubScreen_ToGame, 2);
      } else if (input.PZ()) {
        PracticeMod::GetInstance()->menuActive = !PracticeMod::GetInstance()->menuActive;
      }
    }
    if (!PracticeMod::GetInstance()->menuActive) {
      Orig(self, mgr, input);
    }
  }
};

// CMainFlow::OnMessage
DECLARE_FUNCTION_REPLACEMENT(CMainFlow_OnMessage) {
  static CIOWin::EMessageReturn Callback(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue) {
    if (msg.x4_type == EArchMsgType_UserInput) {
      CArchMsgParmUserInput *status = (CArchMsgParmUserInput *)msg.x8_parm.RawPointer();
      // The mod 4 is just for safety
      PracticeMod::GetInstance()->inputs[status->x4_parm.ControllerIdx() % 4] = status->x4_parm;
      if (status->x4_parm.ControllerIdx() == 0) {
        PracticeMod::GetInstance()->HandleInputs();
        // TODO: move this to hook in Game::Update or something
        PracticeMod::GetInstance()->update(0);
      }
    }

    return Orig(thiz, msg, queue);
  }
};

// CStateManager::DrawDebugStuff
DECLARE_FUNCTION_REPLACEMENT(CStateManager_DrawDebugStuff) {
  static void Callback(CStateManager *self) {
    WorldRenderer::RenderWorld();
  }
};

// CStateManager::Update
DECLARE_FUNCTION_REPLACEMENT(CStateManager_Update) {
  static void Callback(CStateManager *self, float dt) {
    Orig(self, dt);
    PracticeMod::GetInstance()->update(dt);
  }
};

// CScriptSpecialFunction::ShouldSkipCinematic
DECLARE_FUNCTION_REPLACEMENT(CScriptSpecialFunction_ShouldSkipCinematic) {
  static bool Callback(CScriptSpecialFunction *self, const CStateManager &mgr) {
    return true;
  }
};

// CMapWorldInfo::IsMapped
DECLARE_FUNCTION_REPLACEMENT(CMapWorldInfo_IsMapped) {
  static bool Callback(CMapWorldInfo *self, TAreaId areaId) {
    return true;
  }
};

// CMapWorldInfo::IsWorldVisible
DECLARE_FUNCTION_REPLACEMENT(CMapWorldInfo_IsWorldVisible) {
  static bool Callback(CMapWorldInfo *self, TAreaId areaId) {
    return true;
  }
};

// CMapWorldInfo::IsAreaVisible
DECLARE_FUNCTION_REPLACEMENT(CMapWorldInfo_IsAreaVisible) {
  static bool Callback(CMapWorldInfo *self, TAreaId areaId) {
    return true;
  }
};

// CMapArea::GetIsVisibleToAutoMapper
DECLARE_FUNCTION_REPLACEMENT(CMapArea_GetIsVisibleToAutoMapper) {
  static bool Callback(void *, bool, bool) {
    return true;
  }
};

// CAutoMapper::Draw
DECLARE_FUNCTION_REPLACEMENT(CAutoMapper_Draw) {
  static void Callback(CAutoMapper *self, const CStateManager &mgr, const CTransform4f &xf, float alpha) {
    Orig(self, mgr, xf, alpha);
    EAutoMapperState state = self->state();
    EAutoMapperState nextState = self->nextState();
    PracticeMod::GetInstance()->mapActive = state == EAutoMapperState::MapScreen && nextState == EAutoMapperState::MapScreen;
  }
};

// CAutoMapper::ProcessMapScreenInput
DECLARE_FUNCTION_REPLACEMENT(CAutoMapper_ProcessMapScreenInput) {
  static void Callback(CAutoMapper *self, const CFinalInput &input, CStateManager &mgr) {
    Orig(self, input, mgr);

    EAutoMapperState state = self->state();
    EAutoMapperState nextState = self->nextState();
    if (state == EAutoMapperState::MapScreen && nextState == EAutoMapperState::MapScreen) {
      if (input.PX()) {
        IWorld *world = self->world();
        u32 worldid = world->IGetWorldAssetId();
        TAreaId areaId = *self->curAreaId();
        IGameArea *area = world->IGetAreaAlways(areaId);
        u32 mrea = area->IGetAreaAssetId();
        warp(worldid, mrea);
      }
    }
  }
};

DECLARE_FUNCTION_REPLACEMENT(CRandom16_Next) {
  static s32 Callback(CRandom16* rng) {
    if (!SETTINGS.RNG_lockSeed) {
      rng->m_seed = (rng->m_seed * 0x41c64e6d) + 0x00003039;
    }
    return (rng->m_seed >> 16) & 0xffff;
  }
};

#ifdef DEBUG
// CMainFlow::AdvanceGameState
DECLARE_FUNCTION_REPLACEMENT(CMainFlow_AdvanceGameState) {
 static void Callback(CMainFlow *self, CArchitectureQueue &queue) {
  DebugLog("Hooked advance game state\n");
  // This hook is only compiled in if mode == Debug

  // Hook into CMainFlow::AdvanceGameState(). When this function is called with
  // the game state set to PreFrontEnd, that indicates that engine initialization
  // is complete and the game is proceeding to the main menu. We hook in here to
  // bypass the main menu and boot directly into the game.
  static bool sHasDoneInitialBoot = false;

  // Make sure the patch does not run twice if the player quits out to main menu
  if (!sHasDoneInitialBoot && self->GetGameState() == 7) {
    sHasDoneInitialBoot = true;
    CGameState *gameState = *((CGameState **)(0x80457798 + 0x134));
    gameState->SetCurrentWorldId(0x83F6FF6F);                         // chozo ruins
    gameState->CurrentWorldState().SetDesiredAreaAssetId(0x47E73BC5); // gathering hall
    self->SetGameState(kCFS_Game, queue);
  } else {
    Orig(self, queue);
  }
}
};
#endif

// clang-format on

void InstallHooks() {
  CGraphics_EndScene::InstallAtFuncPtr(&CGraphics::EndScene);
  CPlayerGun_DropBomb::InstallAtFuncPtr(&CPlayerGun::DropBomb);
  CPauseScreen_ProcessControllerInput::InstallAtFuncPtr(&CPauseScreen::ProcessControllerInput);
  CMainFlow_OnMessage::InstallAtFuncPtr(&CMainFlow::OnMessage);
  CStateManager_DrawDebugStuff::InstallAtFuncPtr(&CStateManager::DrawDebugStuff);
  CStateManager_Update::InstallAtFuncPtr(&CStateManager::Update);
  CScriptSpecialFunction_ShouldSkipCinematic::InstallAtFuncPtr(&CScriptSpecialFunction::ShouldSkipCinematic);
  CMapWorldInfo_IsMapped::InstallAtFuncPtr(&CMapWorldInfo::IsMapped);
  CMapWorldInfo_IsWorldVisible::InstallAtFuncPtr(&CMapWorldInfo::IsWorldVisible);
  CMapWorldInfo_IsAreaVisible::InstallAtFuncPtr(&CMapWorldInfo::IsAreaVisible);
  CMapArea_GetIsVisibleToAutoMapper::InstallAtFuncPtr(&CMapArea::GetIsVisibleToAutoMapper);
  CAutoMapper_Draw::InstallAtFuncPtr(&CAutoMapper::Draw);
  CAutoMapper_ProcessMapScreenInput::InstallAtFuncPtr(&CAutoMapper::ProcessMapScreenInput);
  CRandom16_Next::InstallAtFuncPtr(&CRandom16::Next);
#ifdef DEBUG
  CMainFlow_AdvanceGameState::InstallAtFuncPtr(&CMainFlow::AdvanceGameState);
#endif
}
