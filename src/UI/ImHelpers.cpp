#include "ImHelpers.hpp"

#include "imgui_internal.h"

namespace ImHelpers {
  void ClampCurrentWindowToScreen() {
    // check if window is offscreen and move it onscreen
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    ImVec2 windowPos = window->Pos;
    ImVec2 windowSize = window->Size;

    float padding = 10.0f;
    bool moved = false;
    if (windowPos.x < padding) {
      windowPos.x = padding;
      moved = true;
    }
    if (windowPos.x + windowSize.x  + padding > io.DisplaySize.x) {
      windowPos.x = io.DisplaySize.x - windowSize.x - padding;
      moved = true;
    }
    if (windowPos.y < padding) {
      windowPos.y = padding;
      moved = true;
    }
    if (windowPos.y + windowSize.y + padding > io.DisplaySize.y) {
      windowPos.y = io.DisplaySize.y - windowSize.y - padding;
      moved = true;
    }

    if (moved) {
      ImGui::SetWindowPos(window->Name, windowPos);
    }
  }
}

