#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include "PlayerMenu.hpp"
#include "imgui.h"

namespace GUI {
  void drawPlayerMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    float x = player->GetPhysicsState().x0_translation.x;
    float y = player->GetPhysicsState().x0_translation.y;
    float z = player->GetPhysicsState().x0_translation.z;

    ImGuiSliderFlags flags = ImGuiSliderFlags_None;
    if (ImGui::TreeNode("Player")) {
      ImGui::DragFloat("X", &x, 0.05f, FLT_MIN, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Y", &y, 0.05f, FLT_MIN, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Z", &z, 0.05f, FLT_MIN, FLT_MAX, "%.3f", flags);
      ImGui::TreePop();
    }
  }
}

