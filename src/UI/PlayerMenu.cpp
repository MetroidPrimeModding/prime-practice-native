#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerState.hpp>
#include <prime/CGameGlobalObjects.hpp>
#include <prime/CGameState.hpp>
#include <prime/CWorldState.hpp>
#include <prime/CWorld.hpp>
#include "PlayerMenu.hpp"
#include "WarpMenu.h"
#include "imgui.h"

namespace GUI {
  CTransform4f savedPos{CTransform4f::Identity()};
  CVector3f savedVelocity{};
  CVector3f savedAngularVelocity{};
  u32 savedWorldAssetID{0};
  u32 savedAreaAssetID{0};

  void drawPlayerMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    CGameGlobalObjects *globals = ((CGameGlobalObjects *) 0x80457798);
    CGameState *gameState = globals->x134_gameState;

    u32 currentWorldAssetID = gameState->MLVL();
    u32 currentAreaAssetID = 0;
    CWorld *world = stateManager->GetWorld();
    if (world) {
      currentAreaAssetID = world->areas()->ptr[gameState->CurrentWorldState().x4_areaId].ptr->mrea();
    }
    if (savedWorldAssetID == 0 || savedWorldAssetID == 0xFFFFFFFF) {
      savedWorldAssetID = currentWorldAssetID;
    }
    if (savedAreaAssetID == 0 || savedAreaAssetID == 0xFFFFFFFF) {
      savedAreaAssetID = currentAreaAssetID;
    }

    ImGuiSliderFlags flags = ImGuiSliderFlags_None
                             | ImGuiSliderFlags_NoRoundToFormat;
    if (ImGui::TreeNode("Player")) {
      if (ImGui::Button("Save position")) {
        savedPos = *player->getTransform();
        savedVelocity = *player->GetVelocity();
        savedAngularVelocity = *player->GetAngularVelocity();
        savedWorldAssetID = currentWorldAssetID;
        savedAreaAssetID = currentAreaAssetID;
      }
      ImGui::SameLine();
      if (ImGui::Button("Load position")) {
        *player->getTransform() = savedPos;
        *player->GetVelocity() = savedVelocity;
        *player->GetAngularVelocity() = savedAngularVelocity;
        if (currentWorldAssetID != savedWorldAssetID || currentAreaAssetID != savedAreaAssetID) {
          warp(savedWorldAssetID, savedAreaAssetID);
        }
      }
      ImGui::Text("Saved position: %.2fx, %.2fy, %.2fz", savedPos.x, savedPos.y, savedPos.z);
      const char *worldName = getNameForWorldAsset(savedWorldAssetID);
      const char *areaName = getNameForAreaAsset(savedWorldAssetID, savedAreaAssetID);
      ImGui::Text("%s", worldName);
      ImGui::Text("%s", areaName);

      ImGui::DragFloat("X", &player->getTransform()->x, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Y", &player->getTransform()->y, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::DragFloat("Z", &player->getTransform()->z, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      ImGui::TreePop();
    }
  }
}

