#include <imgui.h>
#include <prime/CGameState.hpp>
#include "InventoryMenu.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CWorldState.hpp"

static constexpr CPlayerState::EItemType GeneralItems[] = {
    CPlayerState::EItemType::EnergyTanks, CPlayerState::EItemType::CombatVisor, CPlayerState::EItemType::ScanVisor,
    CPlayerState::EItemType::ThermalVisor, CPlayerState::EItemType::XRayVisor, CPlayerState::EItemType::GrappleBeam,
    CPlayerState::EItemType::SpaceJumpBoots, CPlayerState::EItemType::PowerSuit, CPlayerState::EItemType::VariaSuit,
    CPlayerState::EItemType::GravitySuit, CPlayerState::EItemType::PhazonSuit,
};

static constexpr CPlayerState::EItemType WeaponItems[] = {
    CPlayerState::EItemType::Missiles, CPlayerState::EItemType::PowerBeam, CPlayerState::EItemType::IceBeam,
    CPlayerState::EItemType::WaveBeam, CPlayerState::EItemType::PlasmaBeam, CPlayerState::EItemType::SuperMissile,
    CPlayerState::EItemType::Flamethrower, CPlayerState::EItemType::IceSpreader, CPlayerState::EItemType::Wavebuster,
    CPlayerState::EItemType::ChargeBeam,
};

static constexpr CPlayerState::EItemType MorphBallItems[] = {
    CPlayerState::EItemType::PowerBombs, CPlayerState::EItemType::MorphBall, CPlayerState::EItemType::MorphBallBombs,
    CPlayerState::EItemType::BoostBall, CPlayerState::EItemType::SpiderBall,
};

static constexpr CPlayerState::EItemType ArtifactItems[] = {
    CPlayerState::EItemType::Truth, CPlayerState::EItemType::Strength, CPlayerState::EItemType::Elder,
    CPlayerState::EItemType::Wild, CPlayerState::EItemType::Lifegiver, CPlayerState::EItemType::Warrior,
    CPlayerState::EItemType::Chozo, CPlayerState::EItemType::Nature, CPlayerState::EItemType::Sun,
    CPlayerState::EItemType::World, CPlayerState::EItemType::Spirit, CPlayerState::EItemType::Newborn,
};

namespace GUI {
  void RenderItemType(CPlayerState *playerState, CPlayerState::EItemType itemType);
  const char *ItemTypeToName(CPlayerState::EItemType type);
  static inline void RenderItemsDualColumn(CPlayerState *playerState, const CPlayerState::EItemType *items, int start, int end);

  void drawInventoryMenu() {
    CStateManager *stateManager = CStateManager_INSTANCE;
//    CPlayer *player = stateManager->Player();
    CPlayerState *playerState = stateManager->GetPlayerState();

    if (ImGui::TreeNode("Inventory")) {
      if (ImGui::Button("Refill")) {
        for (int i = 0; i < int(CPlayerState::EItemType::Max); i++) {
          auto itemType = static_cast<CPlayerState::EItemType>(i);
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
        for (int i = 0; i < int(CPlayerState::EItemType::Max); i++) {
          auto itemType = static_cast<CPlayerState::EItemType>(i);
          u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
          playerState->ReInitializePowerUp(itemType, maxValue);
          playerState->ResetAndIncrPickUp(itemType, maxValue);
        }
      }
      ImGui::SameLine();
      if (ImGui::Button("None")) {
        for (int i = 0; i < int(CPlayerState::EItemType::Max); i++) {
          auto itemType = static_cast<CPlayerState::EItemType>(i);
          playerState->ReInitializePowerUp(itemType, 0);
        }
      }

      if (ImGui::BeginTabBar("Items")) {
        if (ImGui::BeginTabItem("General")) {
          RenderItemType(playerState, GeneralItems[0]); // full width
          RenderItemsDualColumn(playerState, GeneralItems, 1, sizeof(GeneralItems) / sizeof(GeneralItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Weapons")) {
          RenderItemType(playerState, WeaponItems[0]); // full width
          RenderItemsDualColumn(playerState, WeaponItems, 1, sizeof(WeaponItems) / sizeof(WeaponItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Morph Ball")) {
          RenderItemType(playerState, MorphBallItems[0]); // full width
          RenderItemsDualColumn(playerState, MorphBallItems, 1, sizeof(MorphBallItems) / sizeof(MorphBallItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Artifacts")) {
          ImGui::Text("NOTE: This doesn't affect Artifact Temple layers");
//          ImGui::Text("Use the Layers window to set them for progression");
          RenderItemsDualColumn(playerState, ArtifactItems, 0, sizeof(ArtifactItems) / sizeof(ArtifactItems[0]));
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }

      ImGui::TreePop();
    }
  }

  void RenderItemType(CPlayerState *playerState, CPlayerState::EItemType itemType) {
    u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
    const char *name = ItemTypeToName(itemType);
    if (maxValue == 1) {
      bool enabled = playerState->GetItemCapacity(itemType) == 1;
      if (ImGui::Checkbox(name, &enabled)) {
        if (enabled) {
          playerState->ReInitializePowerUp(itemType, 1);
          playerState->ResetAndIncrPickUp(itemType, 1);
        } else {
          playerState->ReInitializePowerUp(itemType, 0);
        }
        if (itemType == CPlayerState::EItemType::VariaSuit || itemType == CPlayerState::EItemType::PowerSuit ||
            itemType == CPlayerState::EItemType::GravitySuit || itemType == CPlayerState::EItemType::PhazonSuit) {
          // TODO
//          g_StateManager->Player()->AsyncLoadSuit(*g_StateManager);
        }
      }
    } else if (maxValue > 1) {
      int capacity = int(playerState->GetItemCapacity(itemType));
      float speed = 1;
      if (maxValue > 100) {
        speed = 5;
      }
      if (ImGui::DragInt(name, &capacity, speed, 0, int(maxValue), "%d", ImGuiSliderFlags_AlwaysClamp)) {
        playerState->ReInitializePowerUp(itemType, u32(capacity));
        playerState->ResetAndIncrPickUp(itemType, u32(capacity));
      }
    }
  }

  const char *ItemTypeToName(CPlayerState::EItemType type) {
    switch (type) {
      case CPlayerState::EItemType::PowerBeam:
        return "Power Beam";
      case CPlayerState::EItemType::IceBeam:
        return "Ice Beam";
      case CPlayerState::EItemType::WaveBeam:
        return "Wave Beam";
      case CPlayerState::EItemType::PlasmaBeam:
        return "Plasma Beam";
      case CPlayerState::EItemType::Missiles:
        return "Missiles";
      case CPlayerState::EItemType::ScanVisor:
        return "Scan Visor";
      case CPlayerState::EItemType::MorphBallBombs:
        return "Morph Ball Bombs";
      case CPlayerState::EItemType::PowerBombs:
        return "Power Bombs";
      case CPlayerState::EItemType::Flamethrower:
        return "Flamethrower";
      case CPlayerState::EItemType::ThermalVisor:
        return "Thermal Visor";
      case CPlayerState::EItemType::ChargeBeam:
        return "Charge Beam";
      case CPlayerState::EItemType::SuperMissile:
        return "Super Missile";
      case CPlayerState::EItemType::GrappleBeam:
        return "Grapple Beam";
      case CPlayerState::EItemType::XRayVisor:
        return "X-Ray Visor";
      case CPlayerState::EItemType::IceSpreader:
        return "Ice Spreader";
      case CPlayerState::EItemType::SpaceJumpBoots:
        return "Space Jump Boots";
      case CPlayerState::EItemType::MorphBall:
        return "Morph Ball";
      case CPlayerState::EItemType::CombatVisor:
        return "Combat Visor";
      case CPlayerState::EItemType::BoostBall:
        return "Boost Ball";
      case CPlayerState::EItemType::SpiderBall:
        return "Spider Ball";
      case CPlayerState::EItemType::PowerSuit:
        return "Power Suit";
      case CPlayerState::EItemType::GravitySuit:
        return "Gravity Suit";
      case CPlayerState::EItemType::VariaSuit:
        return "Varia Suit";
      case CPlayerState::EItemType::PhazonSuit:
        return "Phazon Suit";
      case CPlayerState::EItemType::EnergyTanks:
        return "Energy Tanks";
      case CPlayerState::EItemType::HealthRefill:
        return "Health Refill";
      case CPlayerState::EItemType::Wavebuster:
        return "Wavebuster";
      case CPlayerState::EItemType::Truth:
        return "Artifact of Truth";
      case CPlayerState::EItemType::Strength:
        return "Artifact of Strength";
      case CPlayerState::EItemType::Elder:
        return "Artifact of Elder";
      case CPlayerState::EItemType::Wild:
        return "Artifact of Wild";
      case CPlayerState::EItemType::Lifegiver:
        return "Artifact of Lifegiver";
      case CPlayerState::EItemType::Warrior:
        return "Artifact of Warrior";
      case CPlayerState::EItemType::Chozo:
        return "Artifact of Chozo";
      case CPlayerState::EItemType::Nature:
        return "Artifact of Nature";
      case CPlayerState::EItemType::Sun:
        return "Artifact of Sun";
      case CPlayerState::EItemType::World:
        return "Artifact of World";
      case CPlayerState::EItemType::Spirit:
        return "Artifact of Spirit";
      case CPlayerState::EItemType::Newborn:
        return "Artifact of Newborn";
      default:
        return "[unknown]";
    }
  }

  void RenderItemsDualColumn(CPlayerState *playerState, const CPlayerState::EItemType *items, int start, int end) {
    ImGui::BeginGroup();
    // Render left group
    for (int i = start; i < end; i += 2) {
      RenderItemType(playerState, items[i]);
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    // Render right group
    for (int i = start + 1; i < end; i += 2) {
      RenderItemType(playerState, items[i]);
    }
    ImGui::EndGroup();
  }
}
