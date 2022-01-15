#include <imgui.h>
#include "ImHelpers.hpp"
#include "SettingsMenu.hpp"
#include "settings.hpp"
#include "BombJumping.hpp"

namespace GUI {
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

        // end in-game display
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Bomb Jumping")) {
        BITFIELD_CHECKBOX("Timing and position UI", SETTINGS.BOMBJUMP_enable);
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
        ImGui::Text("Lag:");
        ImGui::SliderInt("Loops", &SETTINGS.LAG_loop_iterations, 0, 30000, "%d",
                         ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderInt("Tris", &SETTINGS.LAG_tri_renders, 0, 10000, "%d",
                         ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_AlwaysClamp);
        ImGui::TreePop();
      }

      // End settings menu
      ImGui::TreePop();
    }
  }
}

