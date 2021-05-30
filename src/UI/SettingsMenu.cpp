#include <imgui.h>
#include "ImHelpers.hpp"
#include "SettingsMenu.hpp"
#include "settings.hpp"

namespace GUI {
  void drawSettingsMenu() {
    if (ImGui::TreeNode("Settings")) {
      if (ImGui::TreeNode("In-game display")) {
        BITFIELD_CHECKBOX("Show", SETTINGS.showInGame);
        BITFIELD_CHECKBOX("Input", SETTINGS.showInput);
        BITFIELD_CHECKBOX("Frame time", SETTINGS.showFrameTime);
        BITFIELD_CHECKBOX("Memory info", SETTINGS.showMemoryInfo);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Memory graph", SETTINGS.showMemoryGraph);

        // end in-game display
        ImGui::TreePop();
      }
      // End settings menu
      ImGui::TreePop();
    }
  }
}

