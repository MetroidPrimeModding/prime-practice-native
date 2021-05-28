#include <imgui.h>
#include <prime/CGameState.hpp>
#include "InventoryMenu.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CWorldState.hpp"

static constexpr CPlayerState::EItemType ItemOrder[] = {
    CPlayerState::kItem_PowerBeam,
    CPlayerState::kItem_ChargeBeam,
    CPlayerState::kItem_IceBeam,
    CPlayerState::kItem_WaveBeam,
    CPlayerState::kItem_PlasmaBeam,
    CPlayerState::kItem_EnergyTank,
    CPlayerState::kItem_Missile,
    CPlayerState::kItem_SuperMissile,
    CPlayerState::kItem_Flamethrower,
    CPlayerState::kItem_IceSpreader,
    CPlayerState::kItem_Wavebuster,
    CPlayerState::kItem_CombatVisor,
    CPlayerState::kItem_ScanVisor,
    CPlayerState::kItem_ThermalVisor,
    CPlayerState::kItem_XRayVisor,
    CPlayerState::kItem_MorphBall,
    CPlayerState::kItem_MorphBallBomb,
    CPlayerState::kItem_PowerBomb,
    CPlayerState::kItem_BoostBall,
    CPlayerState::kItem_SpiderBall,
    CPlayerState::kItem_GrappleBeam,
    CPlayerState::kItem_SpaceJump,
    CPlayerState::kItem_PowerSuit,
    CPlayerState::kItem_VariaSuit,
    CPlayerState::kItem_GravitySuit,
    CPlayerState::kItem_PhazonSuit,
    CPlayerState::kItem_ArtifactOfTruth,
    CPlayerState::kItem_ArtifactOfStrength,
    CPlayerState::kItem_ArtifactOfElder,
    CPlayerState::kItem_ArtifactOfWild,
    CPlayerState::kItem_ArtifactOfLifegiver,
    CPlayerState::kItem_ArtifactOfWarrior,
    CPlayerState::kItem_ArtifactOfChozo,
    CPlayerState::kItem_ArtifactOfNature,
    CPlayerState::kItem_ArtifactOfSun,
    CPlayerState::kItem_ArtifactOfWorld,
    CPlayerState::kItem_ArtifactOfSpirit,
    CPlayerState::kItem_ArtifactOfNewborn,
};

namespace GUI {
  void drawInventoryMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    if (ImGui::TreeNode("Inventory")) {
      if (ImGui::Button("Refill")) {
        for (const auto itemType : ItemOrder) {
          u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
          playerState->ResetAndIncrPickUp(itemType, maxValue);
        }
      }
//      {
//        ImGui::SameLine();
//        auto& mapWorldInfo = *g_GameState->CurrentWorldState().MapWorldInfo();
//        bool mapStationUsed = mapWorldInfo.GetMapStationUsed();
//        if (ImGui::Checkbox("Area map", &mapStationUsed)) {
//          mapWorldInfo.SetMapStationUsed(mapStationUsed);
//        }
//      }
      ImGui::SameLine();
      if (ImGui::Button("All")) {
        for (const auto itemType : ItemOrder) {
          u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
          playerState->ReInitializePowerUp(itemType, maxValue);
          playerState->ResetAndIncrPickUp(itemType, maxValue);
        }
      }
      ImGui::SameLine();
      if (ImGui::Button("None")) {
        for (const auto itemType : ItemOrder) {
          playerState->ReInitializePowerUp(itemType, 0);
        }
      }
      ImGui::TreePop();
    }
  }
}
