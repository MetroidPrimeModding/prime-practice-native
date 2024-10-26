#include "RoomMenu.hpp"

#include <imgui.h>
#include "ImHelpers.hpp"
#include "os.h"
#include "prime/CStateManager.hpp"
#include "prime/CWorld.hpp"
#include "WarpMenu.h"


namespace GUI {
  void hideAllRooms();
  void showVisibilityControls();

  void showLoadControls();

  void drawRoomMenu() {
    if (ImGui::TreeNode("Rooms")) {

      if (ImGui::TreeNode("Visibility")) {
        ImGui::Text("Warning: If you show too many rooms, the game may crash!");
        if (ImGui::Button("Hide all")) {
          hideAllRooms();
        }
        showVisibilityControls();
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Load")) {
        ImGui::Text("Warning: If you load too many rooms, the game may crash!");
        showLoadControls();
        ImGui::TreePop();
      }
      ImGui::TreePop();
    }
  }

  void hideAllRooms() {
    CStateManager *stateManager = CStateManager::instance();
    CWorld *world = stateManager->GetWorld();
    if (world == nullptr) {
      return;
    }
    TAreaId areaId = world->IGetCurrentAreaId();
    auto areas = world->areas();
    for (int i = 0; i < areas->length(); i++) {
      auto area = areas->get(i).ptr;
      if (area->areaIDx() == areaId.id) continue;
      area->SetOcclusionState(EOcclusionState::Occluded);
    }
  }

  void showVisibilityControls() {
    CStateManager *stateManager = CStateManager::instance();
    CWorld *world = stateManager->GetWorld();
    if (world == nullptr) {
      return;
    }
    TAreaId areaId = world->IGetCurrentAreaId();
    auto areas = world->areas();
    for (int i = 0; i < areas->length(); i++) {
      auto area = areas->get(i).ptr;
      auto post = area->postConstructed();
      if (post != nullptr && area->curChain() == EChain::Alive) {
        if (area->areaIDx() == areaId.id) continue;
        const char *name = getNameForAreaAsset(world->IGetWorldAssetId(), area->IGetAreaAssetId());
        ImGui::Text("%s", name);
        ImGui::SameLine();
        if (post->occlusionState() == EOcclusionState::Occluded) {
          char label[32];
          memset(label, 0, 32);
          snprintf(label, 32, "Show###%d", area->IGetAreaAssetId());
          if (ImGui::Button(label)) {
            area->SetOcclusionState(EOcclusionState::Visible);
          }
        } else {
          char label[32];
          memset(label, 0, 32);
          snprintf(label, 32, "Hide###%d", area->IGetAreaAssetId());
          if (ImGui::Button(label)) {
            area->SetOcclusionState(EOcclusionState::Occluded);
          }
        }
      }
    }
  }

  void showLoadControls() {
    CStateManager *stateManager = CStateManager::instance();
    CWorld *world = stateManager->GetWorld();
    if (world == nullptr) {
      return;
    }
    TAreaId areaId = world->IGetCurrentAreaId();
    auto currentArea = world->areas()->get(areaId.id).ptr;
    auto docks = currentArea->docks();
    for (int i = 0; i < docks->length(); i++) {
      auto dock = &docks->get(i);
      auto refs = dock->refs();
      for (int j = 0; j < refs->length(); j++) {
        auto ref = &refs->get(j);
        auto connectedAreaId = ref->x0_area;
        auto connectedArea = world->areas()->get(connectedAreaId.id).ptr;
        const char *name = getNameForAreaAsset(world->IGetWorldAssetId(), connectedArea->IGetAreaAssetId());
        ImGui::Text("%s %x", name, ref->x6_loadOther);
        ImGui::SameLine();
        bool loadOther = (ref->x6_loadOther & 0x80) > 0;
        if (loadOther) {
          char label[32];
          memset(label, 0, 32);
          snprintf(label, 32, "Unload###%d", connectedAreaId.id);
          if (ImGui::Button(label)) {
            ref->x6_loadOther &= 0x7F;
          }
        } else {
          char label[32];
          memset(label, 0, 32);
          snprintf(label, 32, "Load###%d", connectedAreaId.id);
          if (ImGui::Button(label)) {
            ref->x6_loadOther |= 0x80;
          }
        }
      }
    }
  }
}

