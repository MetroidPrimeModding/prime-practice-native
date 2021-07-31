#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerState.hpp>
#include <prime/CGameGlobalObjects.hpp>
#include <prime/CGameState.hpp>
#include <prime/CWorldState.hpp>
#include <prime/CWorld.hpp>
#include <prime/CCameraBobber.hpp>
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
      // TODO: when I figure out how to save both position and room, change this
      if (currentWorldAssetID != savedWorldAssetID || currentAreaAssetID != savedAreaAssetID) {
        const char *worldName = getNameForWorldAsset(savedWorldAssetID);
        const char *areaName = getNameForAreaAsset(savedWorldAssetID, savedAreaAssetID);
        ImGui::Text("Saved position:");
        ImGui::Text("%s", worldName);
        ImGui::Text("%s", areaName);
      } else {
        ImGui::Text("Saved position: %.2fx, %.2fy, %.2fz", savedPos.x, savedPos.y, savedPos.z);
      }

      float xyz[3] = {
          player->getTransform()->x,
          player->getTransform()->y,
          player->getTransform()->z
      };

      ImGui::DragFloat3("", xyz, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);
      player->getTransform()->x = xyz[0];
      player->getTransform()->y = xyz[1];
      player->getTransform()->z = xyz[2];

      if (ImGui::Button("IS on")) {
        player->GetAngularVelocity()->x = NAN;
        player->GetAngularVelocity()->y = NAN;
        player->GetAngularVelocity()->z = NAN;
      }
      ImGui::SameLine();
      ImGui::Text("Use while morphed");

      auto *bobber = player->getCameraBobber();
      if (ImGui::Button("Lightshow fix")) {
        player->GetAngularVelocity()->x = 0;
        player->GetAngularVelocity()->y = 0;
        player->GetAngularVelocity()->z = 0;


        *(bobber->getTargetBobMagnitude()) = 0;
        *(bobber->getBobMagnitude()) = 0;
        *(bobber->getBobTimescale()) = 0;
        *(bobber->getBobTime()) = 0;
        bobber->getCameraBobTransform()->x = 0;
        bobber->getCameraBobTransform()->y = 0;
        bobber->getCameraBobTransform()->z = 0;
      }

      ImGui::SameLine();
      ImGui::Text("Use while unmorphed");

      ImGui::TreePop();
    }
  }
}

