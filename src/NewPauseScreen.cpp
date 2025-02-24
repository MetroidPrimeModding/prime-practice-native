#include "UI/PlayerMenu.hpp"
#include "UI/InventoryMenu.hpp"
#include "UI/MonitorWindow.hpp"
#include "UI/SettingsMenu.hpp"
#include "UI/BombJumping.hpp"
#include "os.h"
#include "NewPauseScreen.hpp"
#include "prime/CGameState.hpp"
#include "prime/CWorldState.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CPlayerGun.hpp"
#include "prime/CWorld.hpp"
#include "prime/CMain.hpp"
#include "prime/CSfxManager.hpp"
#include "imgui.h"
#include "duk_mem.h"
#include "UI/WarpMenu.h"
#include "settings.hpp"
#include "ImGuiEngine.hpp"
#include "version.h"
#include "UI/RoomMenu.hpp"
#include "UI/QR.hpp"
#include "UI/DumpMemoryUI.hpp"
#include "utils.hpp"

#define PAD_MAX_CONTROLLERS 4

NewPauseScreen *NewPauseScreen::instance = nullptr;

NewPauseScreen::NewPauseScreen() {
  OSReport("Hello, Dolphin\n");

  ImGuiEngine::ImGui_Init();
  ImGuiEngine::ImGui_Init_Style();
  GUI::initQR();
  this->hide();
  inputs = new CFinalInput[4];

  // Patch CScriptTrigger so we can attach a value to it
  // CScriptTrigger::CScriptTrigger
  // 50A03E30 rlwimi r0, r5, 7, 24, 24
  // 98140148 stb r0, 0x148(r20)
  // lbz r0, 0x148(r20) 0x88140148

  *((u32 *) 0x80076f0c) = 0x90940148; // stw r4, 0x148(r20)
  *((u32 *) 0x80076f10) = 0x50A03E30; // rlwimi r0, r5, 7, 24, 24
  *((u32 *) 0x80076f14) = 0x98140148; // stb r0, 0x148(r20)

  // Patch file select IGT to mm:ss
  // r5 contains number of seconds
  // we want 5 and 6 as out, so

  // r7 = 60
  // r7 = r5 / r7 (seconds / 60) = minutes
  // r6 = r7 * 60 (minutes * 60)
  // r6 = r5 - r6 (seconds - (minutes * 60))
  // r5 = r7 // minutes
  /*
  li 7, 60
  divw 7, 5, 7
  mulli 6, 7, 60
  sub 6, 5, 6
  mr 5, 7
   */

  // nop out our region and overwrite
  for (u32 i = 0x8001FEF4; i <= 0x8001FF38; i += 4) {
    *((u32 *) i) = 0x60000000; // nop
  }
  *((u32 *) 0x8001FEF4) = 0x38e0003C; // li r7, 60
  *((u32 *) 0x8001FEF8) = 0x7CE53BD6; // divw r7, r5, r7
  *((u32 *) 0x8001FEFC) = 0x1CC7003C; // muli r6, r7, 60
  *((u32 *) 0x8001FF00) = 0x7CC62850; // sub r6, r5, r6
  *((u32 *) 0x8001FF04) = 0x7CE53B78; // mr r5, r7

  // Swap what text is used for ELAPSED to blank
  *((u32 *) 0x8001FFB8) = 0x3880005C; // li r4, 92 - which is blank

  // This will cause the crash screen to appear every time
  *((u32 *) 0x802d6a44) = 0x60000000;

#if DEBUG
  // force splash disabled
  *((u32*)0x8003732c) = 0x38000001; // li, r0, 1
#endif
}

void NewPauseScreen::Render() {
  NewPauseScreen::RenderMenu();
}

void NewPauseScreen::hide() {
  pauseScreenActive = false;
}

void NewPauseScreen::show() {
  this->pauseScreenActive = true;
}

void NewPauseScreen::HandleInputs() {
  if (this->pauseScreenActive && this->menuActive) {
    ImGuiIO *io = &ImGui::GetIO();
    io->NavInputs[ImGuiNavInput_Activate] = inputs[0].DA();
    io->NavInputs[ImGuiNavInput_Cancel] = inputs[0].DB();
//    io->NavInputs[ImGuiNavInput_Menu] = inputs[0].DX();
//    io->NavInputs[ImGuiNavInput_Input] = inputs[0].DY();

    // dpad
    io->NavInputs[ImGuiNavInput_DpadLeft] = inputs[0].DDPLeft() || inputs[0].DLALeft();
    io->NavInputs[ImGuiNavInput_DpadRight] = inputs[0].DDPRight() || inputs[0].DLARight();
    io->NavInputs[ImGuiNavInput_DpadUp] = inputs[0].DDPUp() || inputs[0].DLAUp();
    io->NavInputs[ImGuiNavInput_DpadDown] = inputs[0].DDPDown() || inputs[0].DLADown();
    // analog
    io->NavInputs[ImGuiNavInput_LStickLeft] = inputs[0].ARALeft();
    io->NavInputs[ImGuiNavInput_LStickRight] = inputs[0].ARARight();
    io->NavInputs[ImGuiNavInput_LStickUp] = inputs[0].ARAUp();
    io->NavInputs[ImGuiNavInput_LStickDown] = inputs[0].ARADown();


    /*
    MAP_BUTTON(ImGuiNavInput_FocusPrev,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
     */
  } else if (!this->pauseScreenActive) {
    //  warp hotkeys
    if (hotkeyInputTimeout <= 0) {
      upPresses = 0;
      downPresses = 0;
    } else {
      hotkeyInputTimeout -= 1.f / 60.f;
    }

    if (inputs[0].PDPUp()) {
      upPresses++;
      if (hotkeyInputTimeout <= 0) hotkeyInputTimeout = 2.0f;
    }
    if (inputs[0].PDPDown()) {
      downPresses++;
      if (hotkeyInputTimeout <= 0) hotkeyInputTimeout = 2.0f;
    }

    if (upPresses >= 3) {
      GUI::loadPos();
      hotkeyInputTimeout = 0;
    }
    if (downPresses >= 2) {
      GUI::savePos();
      hotkeyInputTimeout = 0;
    }
  }
}

void NewPauseScreen::RenderMenu() {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(SVIEWPORT_GLOBAL->x8_width, SVIEWPORT_GLOBAL->xc_height);
  io.DeltaTime = 1.f / 60.f;

  ImGui::NewFrame();
  GUI::qrNewFrame();
  bool render = this->pauseScreenActive && this->menuActive;
  if (render) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always, ImVec2(0, 0));
  } else {
    // Hack: if we're not rendering, just move off-screen so that it doesn't show up on-screen
    // This allows it to persist selected item state
    ImGui::SetNextWindowPos(ImVec2(1000, 20), ImGuiCond_Always, ImVec2(0, 0));
  }
  ImGui::SetNextWindowFocus();
  ImGui::SetNextWindowCollapsed(!render, ImGuiCond_Always);
  ImGui::SetNextWindowSizeConstraints(
      ImVec2(0, 0),
      ImVec2(400, SVIEWPORT_GLOBAL->xc_height - 40)
  );
  if (ImGui::Begin(
      "Practice Mod", nullptr,
      ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoMove
  )) {
    GUI::drawPlayerMenu();
    GUI::drawInventoryMenu();
    GUI::drawSettingsMenu();
    GUI::drawWarpMenu();
    GUI::drawRoomMenu();
    if (ImGui::TreeNode("v%s", PRAC_MOD_VERSION)) {
      ImGui::Text("Links (QR codes):");
      if (ImGui::TreeNode("Releases")) {
        GUI::drawQRCode("https://github.com/MetroidPrimeModding/prime-practice-native/releases", 3.0f);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Discord")) {
        GUI::drawQRCode("https://discord.gg/m4UreBdq9V", 3.0f);
        ImGui::TreePop();
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();

  GUI::drawMonitorWindow(inputs);
  GUI::drawBombJumpingInterface();

  if (mapActive) {
    ImGui::SetNextWindowPos(ImVec2(320, 360), ImGuiCond_Always, ImVec2(0.5, 1));
    ImGui::Begin(
        "Hint", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNavInputs |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiFocusedFlags_None // just for conveneint commenting in/out
    );
    ImGui::Text("Press X to warp");
    ImGui::End();
  }

  GUI::drawMemoryDump();

  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();
  ImGuiEngine::ImGui_Render(drawData);

  // Lagger (tris)
  if (!pauseScreenActive && SETTINGS.LAG_tri_renders > 0) {
    int maxIdxPerBatch = 3 * 30;
    int idxPerBatch = 0;
    const float two_pi_thirds = 3.1415f * 2.f / 3.f;
    const float four_pi_thirds = 3.1415 * 4.f / 3.f;
    const float size = 10;
    float theta = 0;
    CGraphics::StreamBegin(ERglPrimitive_TRIANGLES);
    for (int i = 0; i < SETTINGS.LAG_tri_renders; i++) {
      CGraphics::StreamColor(IM_COL32(255, 255, 255, 255));
      CGraphics::StreamTexcoord(io.Fonts->TexUvWhitePixel.x, io.Fonts->TexUvWhitePixel.y);
      CGraphics::StreamVertex(CMath::FastCosR(theta) * size + size, CMath::FastSinR(theta) * size + size, 0);
      CGraphics::StreamVertex(CMath::FastCosR(theta + two_pi_thirds) * size + size,
                              CMath::FastSinR(theta + two_pi_thirds) * size + size,
                              0);
      CGraphics::StreamVertex(CMath::FastCosR(theta + four_pi_thirds / 3) * size + size,
                              CMath::FastSinR(theta + four_pi_thirds / 3) * size + size,
                              0);
      theta += 0.1;
      idxPerBatch += 3;
      if (idxPerBatch > maxIdxPerBatch && idxPerBatch % 3 == 0) {
        idxPerBatch = 0;
        CGraphics::FlushStream();
      }
    }
    idxPerBatch = 0;
    CGraphics::StreamEnd();
  }
}


float bombTime = 0;

void NewPauseScreen::update(float dt) const {
  // Lagger (loops)
  if (!this->pauseScreenActive) {
    if (SETTINGS.LAG_loop_iterations > 0) {
      int c = 1;
      for (int i = 1; i < SETTINGS.LAG_loop_iterations; i++) {
        for (int j = 1; j < 100; j++) {
          c = (c + 1 + i + j) * c - j;
        }
      }
      ImDrawList *dl = ImGui::GetForegroundDrawList();
      char text[64];
      int l = snprintf(text, sizeof(text), "lag value: %d", c);
      dl->AddText(ImVec2(20, 20), IM_COL32(255, 255, 255, 255),
                  text, text + l);
    }
  }

  if (SETTINGS.BOMBJUMP_infiniteBombs) {
    CStateManager *stateManager = CStateManager::instance();
    CPlayer *player = stateManager->Player();
    if (player == nullptr) goto bombjump_done;
    CPlayerGun *gun = player->getPlayerGun();
    if (gun == nullptr) goto bombjump_done;
    u32 maxBomb = readValueFromMemory<u32>(0x80041644) & 0xFFFF;
    *gun->x308_bombCount() = maxBomb;
  }
  bombjump_done:;

}

void warp(uint32_t world, uint32_t area) {
  OSReport("Warping to %x, %x\n", world, area);
  CAssetId worldID = (CAssetId) (world);
  CAssetId areaID = (CAssetId) (area);

  CStateManager *mgr = CStateManager::instance();
  mgr->GetWorld()->SetPauseState(true);

  CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
  CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
  gameState->SetCurrentWorldId(worldID);
  gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);

  CMain *cmain = *((CMain **) 0x805A8C38);
  cmain->SetFlowState(EFlowState_None);

  mgr->SetShouldQuitGame(true);

  NewPauseScreen::instance->hide();
}

// entities
/*duk_ret_t script_getEntities(duk_context *ctx) {
  CStateManager *mgr = CStateManager::instance();
  CObjectList *list = mgr->GetAllObjs();
  if (list == nullptr) {
    duk_push_undefined(ctx);
    return 1;
  }
  duk_uarridx_t visited = 0;
  int id = list->first;

  duk_push_array(ctx);
  while (id != 0xFFFF && visited < list->count) {
    SObjectListEntry entry = list->entries[id & 0x3FF];
    if (!VALID_PTR(entry.entity)) {
      break;
    }
    CEntity *entity = entry.entity;

    duk_push_object(ctx);
    {
      duk_push_int(ctx, entity->getVtablePtr());
      duk_put_prop_string(ctx, -2, "vtable");

      duk_push_int(ctx, (duk_int32_t) entity);
      duk_put_prop_string(ctx, -2, "address");
    }
    duk_put_prop_index(ctx, -2, visited);

    visited++;
    id = entry.next;
  }
  return 1;
}*/
