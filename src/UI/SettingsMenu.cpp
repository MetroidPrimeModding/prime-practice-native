#include "SettingsMenu.hpp"
#include "ImHelpers.hpp"
#include <imgui.h>

#include <prime/CRandom16.hpp>

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

  void drawHex32Editor(const char *title, s32 *value);

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
        BITFIELD_CHECKBOX("Previous Room time (P)", SETTINGS.OSD_showPreviousRoomTime);
        BITFIELD_CHECKBOX("Current Room time (C)", SETTINGS.OSD_showCurrentRoomTime);
        BITFIELD_CHECKBOX("Dock -> Load time (L)", SETTINGS.OSD_showMostRecentDoorToLoadTime);
        BITFIELD_CHECKBOX("Frame time", SETTINGS.OSD_showFrameTime);
        BITFIELD_CHECKBOX("Memory info", SETTINGS.OSD_showMemoryInfo);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Memory graph", SETTINGS.OSD_showMemoryGraph);
        BITFIELD_CHECKBOX("Loads", SETTINGS.OSD_showLoads);
        BITFIELD_CHECKBOX("RNG value", SETTINGS.OSD_showRng);
        BITFIELD_CHECKBOX("Incinerator Drone", SETTINGS.OSD_showIDrone);
        BITFIELD_CHECKBOX("Target Info", SETTINGS.OSD_showTargetInfo);

        // end in-game display
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Bomb Jumping")) {
        BITFIELD_CHECKBOX("Timing and position UI", SETTINGS.BOMBJUMP_enable);
        u32 maxBomb = readValueFromMemory<u32>(0x80041644) & 0xFFFF;
        if (ImGui::SliderInt("Max bomb count", (int *) &maxBomb, 0, 10)) {
          writeValueToMemory<u32>(0x80041644, readValueFromMemory<u32>(0x80041644) & 0xFFFF0000 | maxBomb);
          writeValueToMemory<u32>(0x8003fdd0, readValueFromMemory<u32>(0x8003fdd0) & 0xFFFF0000 | maxBomb);

          CPlayer *player = g_StateManager.Player();
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

      if (ImGui::TreeNode("RNG")) {
        CRandom16 *rng = g_StateManager.GetRandom();
        // ImGui::Text("Address of RNG: %08x", (u32)(&rng->m_seed));

        ImGui::Text("Warning: locking may crash sometimes!");
        BITFIELD_CHECKBOX("Lock RNG value", SETTINGS.RNG_lockSeed);
        drawHex32Editor("RNG value", &rng->m_seed);
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

      if (ImGui::TreeNode("Dump Ram")) {
        ImGui::TextColored(ImVec4(0.9, 0.1, 0.1, 1.0), "Warning! Full screen flashing lights!");
        ImGui::Text("This dumps the entirety of RAM to the\nscreen for decoding from a video recording.\nThis will take several minutes.");
        if (ImGui::Button("Dump Ram")) {
          startMemoryDump();
        }
        ImGui::TreePop();
      }

      // End settings menu
      ImGui::TreePop();
    }
    void drawHex32Editor(const char *title, s32 *value);
  }

  void drawWorldLightOption() {
    CWorld *world = g_StateManager.GetWorld();
    if (world == nullptr) {
      ImGui::Text("World is null");
      return;
    }
    TAreaId areaId = world->IGetCurrentAreaId();
    CGameArea *currentArea = (CGameArea *) world->IGetAreaAlways(areaId);
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

  void drawHex32Editor(const char *title, s32 *value) {
    ImGui::Text("%s", title);
    // draw the 8 characters; current character inverted
    static int current_char = 0;
    for (int i = 0; i < 8; i++) {
      int value_at_i = (*value >> ((7 - i) * 4)) & 0xF;
      if (i > 0) ImGui::SameLine();
      ImGui::TextColored(
        i == current_char ? ImVec4(1, 0, 0, 1) : ImVec4(1, 1, 1, 1),
        "%x", value_at_i
      );
    }

    // a grid of 16 buttons, for 0 - F in hex
    for (int i = 0; i < 16; i++) {
      if (i % 4 != 0) ImGui::SameLine();
      char label[2];
      if (i < 10) {
        label[0] = '0' + i;
      } else {
        label[0] = 'A' + (i - 10);
      }
      label[1] = '\0';
      if (ImGui::Button(label)) {
        // clear the value at current_char
        *value &= ~(0xF << ((7 - current_char) * 4));
        // set the value at current_char to i
        *value |= i << ((7 - current_char) * 4);
        current_char = (current_char + 1) % 8;
      }
    }
    if (ImGui::Button("|##start")) {
      current_char = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("<")) {
      current_char--;
      if (current_char < 0) current_char = 7;
    }
    ImGui::SameLine();
    if (ImGui::Button(">")) {
      current_char++;
      if (current_char > 7) current_char = 0;
    }
    ImGui::SameLine();

    if (ImGui::Button("|##end")) {
      current_char = 7;
    }
  }
}
