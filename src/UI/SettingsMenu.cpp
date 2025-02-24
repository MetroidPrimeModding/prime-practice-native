#include <imgui.h>
#include "ImHelpers.hpp"
#include "SettingsMenu.hpp"
#include "settings.hpp"
#include "BombJumping.hpp"
#include "prime/CGameGlobalObjects.hpp"
#include "prime/CWorld.hpp"
#include "utils.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CPlayerGun.hpp"
#include "DumpMemoryUI.hpp"


namespace GUI {
  void drawWorldLightOption();

  void drawSettingsMenu() {
    if (ImGui::TreeNode("Settings")) {
      if (ImGui::TreeNode("On-screen display")) {
        BITFIELD_CHECKBOX("Show", SETTINGS.OSD_show);
        BITFIELD_CHECKBOX("Pos", SETTINGS.OSD_showPos);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Velocity", SETTINGS.OSD_showVelocity);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Angular Velocity", SETTINGS.OSD_showRotationalVelocity);
        BITFIELD_CHECKBOX("Input", SETTINGS.OSD_showInput);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Time", SETTINGS.OSD_showIGT);
        BITFIELD_CHECKBOX("Previous Room time", SETTINGS.OSD_showPreviousRoomTime);
        BITFIELD_CHECKBOX("Current Room time", SETTINGS.OSD_showCurrentRoomTime);
        BITFIELD_CHECKBOX("Frame time", SETTINGS.OSD_showFrameTime);
        BITFIELD_CHECKBOX("Memory info", SETTINGS.OSD_showMemoryInfo);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Memory graph", SETTINGS.OSD_showMemoryGraph);
        BITFIELD_CHECKBOX("Loads", SETTINGS.OSD_showLoads);
        BITFIELD_CHECKBOX("RNG value", SETTINGS.OSD_showRng);
        BITFIELD_CHECKBOX("Incinerator Drone", SETTINGS.OSD_showIDrone);

        // end in-game display
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Bomb Jumping")) {
        BITFIELD_CHECKBOX("Timing and position UI", SETTINGS.BOMBJUMP_enable);
        u32 maxBomb = readValueFromMemory<u32>(0x80041644) & 0xFFFF;
        if (ImGui::SliderInt("Max bomb count", (int *) &maxBomb, 0, 10)) {
          writeValueToMemory<u32>(0x80041644, readValueFromMemory<u32>(0x80041644) & 0xFFFF0000 | maxBomb);
          writeValueToMemory<u32>(0x8003fdd0, readValueFromMemory<u32>(0x8003fdd0) & 0xFFFF0000 | maxBomb);

          CStateManager *stateManager = CStateManager::instance();
          CPlayer *player = stateManager->Player();
          if (player == nullptr) return;
          CPlayerGun *gun = player->getPlayerGun();
          if (gun == nullptr) return;
          *gun->x308_bombCount() = maxBomb;
        }

        BITFIELD_CHECKBOX("Infinite Bombs", SETTINGS.BOMBJUMP_infiniteBombs);

        if (ImGui::TreeNode("Custom Bomb Jump (p2 b)")) {
          drawCustomBombJumpConfigMenu();
          ImGui::TreePop();
        }
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Triggers")) {
        BITFIELD_CHECKBOX("Load", SETTINGS.TRIGGER_renderLoad);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Door", SETTINGS.TRIGGER_renderDoor);
        BITFIELD_CHECKBOX("Force", SETTINGS.TRIGGER_renderForce);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Camera Hint", SETTINGS.TRIGGER_renderCameraHint);
        BITFIELD_CHECKBOX("Other", SETTINGS.TRIGGER_renderUnknown);

        // end in-game display
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Misc")) {
        // Turns out this isn't actually useful for speedrunners. But I'll leave it for posterity.
        //drawWorldLightOption();

        ImGui::Text("Lag:");
        ImGui::SliderInt("Loops", &SETTINGS.LAG_loop_iterations, 0, 30000, "%d",
                         ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("Tris", &SETTINGS.LAG_tri_renders, 0, 10000, "%d",
                         ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);

        ImGui::TreePop();
      }

      if (ImGui::Button("Dump Ram")) {
        startMemoryDump();
      }

      // End settings menu
      ImGui::TreePop();
    }
  }

  void drawWorldLightOption() {
    CStateManager *stateManager = CStateManager::instance();
    CWorld *world = stateManager->GetWorld();
    if (world == nullptr) {
      ImGui::Text("World is null");
      return;
    }
    TAreaId areaId = world->IGetCurrentAreaId();
    CGameArea *currentArea = (CGameArea*)world->IGetAreaAlways(areaId);
    if (currentArea == nullptr) {
      ImGui::Text("Area is null");
      return;
    }

    // Saftey check to make sure this is actually a CGameArea (this is it's vtable)
    uint32_t vt = GetVtable(currentArea);
    if (vt != 0x803da234) {
      ImGui::Text("Vtable is wrong: %x", vt);
      return;
    };

    CPostConstructed *pConstructed = currentArea->postConstructed();
    if (pConstructed == nullptr) {
      ImGui::Text("pcons is null");
      return;
    }

    float *light = pConstructed->GetWorldLightingLevel();
    ImGui::SliderFloat("Light level", light, 0.0f, 2.0f, "%.3f");
  }
}

