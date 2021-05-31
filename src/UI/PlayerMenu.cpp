#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerState.hpp>
#include "PlayerMenu.hpp"
#include "imgui.h"

namespace GUI {
  float savedX{0}, savedY{0}, savedZ{0};
  void drawPlayerMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    ImGuiSliderFlags flags = ImGuiSliderFlags_None
                             | ImGuiSliderFlags_NoRoundToFormat;
    if (ImGui::TreeNode("Player")) {
      float *x = &player->getTransform()->matrix[3];
      float *y = &player->getTransform()->matrix[7];
      float *z = &player->getTransform()->matrix[11];

      if (ImGui::Button("Save position")) {
        savedX = *x;
        savedY = *y;
        savedZ = *z;
      }
      ImGui::SameLine();
      if (ImGui::Button("Load position")) {
        *x = savedX;
        *y = savedY;
        *z = savedZ;
      }
      ImGui::Text("Saved position: %.2fx, %.2fy, %.2fz", savedX, savedY, savedZ);


      ImGui::DragFloat("X", x, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Y", y, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Z", z, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::TreePop();
    }
  }
}

