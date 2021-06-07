#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerState.hpp>
#include "PlayerMenu.hpp"
#include "imgui.h"

namespace GUI {
  CTransform4f savedPos{CTransform4f::Identity()};
  CVector3f savedVelocity{};
  CVector3f savedAngularVelocity{};

  void drawPlayerMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    ImGuiSliderFlags flags = ImGuiSliderFlags_None
                             | ImGuiSliderFlags_NoRoundToFormat;
    if (ImGui::TreeNode("Player")) {
      if (ImGui::Button("Save position")) {
        savedPos = *player->getTransform();
        savedVelocity = *player->GetVelocity();
        savedAngularVelocity = *player->GetAngularVelocity();
      }
      ImGui::SameLine();
      if (ImGui::Button("Load position")) {
        *player->getTransform() = savedPos;
        *player->GetVelocity() = savedVelocity;
        *player->GetAngularVelocity() = savedAngularVelocity;
      }
      ImGui::Text("Saved position: %.2fx, %.2fy, %.2fz", savedPos.x, savedPos.y, savedPos.z);

      ImGui::DragFloat("X", &player->getTransform()->x, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Y", &player->getTransform()->y, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Z", &player->getTransform()->z, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::TreePop();
    }
  }
}

