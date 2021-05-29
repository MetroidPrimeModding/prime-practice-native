#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerState.hpp>
#include "PlayerMenu.hpp"
#include "imgui.h"

namespace GUI {

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
      ImGui::DragFloat("X", x, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Y", y, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Z", z, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::TreePop();
    }
  }
}

