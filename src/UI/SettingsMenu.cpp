#include <imgui.h>
#include "ImHelpers.hpp"
#include "SettingsMenu.hpp"
#include "settings.hpp"

namespace GUI {
  void drawSettingsMenu() {
    if (ImGui::TreeNode("Settings")) {
      if (ImGui::TreeNode("On-screen display")) {
        BITFIELD_CHECKBOX("Show", SETTINGS.OSD_show);
        BITFIELD_CHECKBOX("Pos", SETTINGS.OSD_showPos);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Velocity", SETTINGS.OSD_showVelocity);
        BITFIELD_CHECKBOX("Input", SETTINGS.OSD_showInput);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Time", SETTINGS.OSD_showIGT);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Room time", SETTINGS.OSD_showRoomTime);
        BITFIELD_CHECKBOX("Frame time", SETTINGS.OSD_showFrameTime);
        BITFIELD_CHECKBOX("Memory info", SETTINGS.OSD_showMemoryInfo);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Memory graph", SETTINGS.OSD_showMemoryGraph);

        // end in-game display
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

      // End settings menu
      ImGui::TreePop();
    }
  }
}

