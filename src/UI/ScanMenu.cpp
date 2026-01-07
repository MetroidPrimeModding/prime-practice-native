#include "ScanMenu.hpp"

#include "ImHelpers.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CStateManager.hpp"
#include "settings.hpp"

#include <imgui.h>

namespace GUI {
  void drawScans();

  void drawScanMenu() {
    if (ImGui::TreeNode("Scans")) {
      BITFIELD_CHECKBOX("Infinite Scans", SETTINGS.SCAN_infiniteScanTime);
      if (SETTINGS.SCAN_infiniteScanTime) {
        ImGui::Indent(20.0f);
        BITFIELD_CHECKBOX("Include important scans", SETTINGS.SCAN_infiniteScanTimeOnImportantScans);
        ImGui::Unindent(20.0f);
      }
      if (ImGui::Button("Reset Scans")) {
        CPlayerState *playerState = g_StateManager.GetPlayerState();
        if (playerState) {
          auto &scanTimes = playerState->scanStates;
          for (int i = 0; i < scanTimes.length(); i++) {
            scanTimes[i].time = 0.0f;
          }
        }
      }
      if (ImGui::Button("Complete Scans")) {
        CPlayerState *playerState = g_StateManager.GetPlayerState();
        if (playerState) {
          auto &scanTimes = playerState->scanStates;
          for (int i = 0; i < scanTimes.length(); i++) {
            scanTimes[i].time = 10.0f;
          }
        }
      }

      // draw a list of all the scans we can scroll thru
      // drawScans();

      ImGui::TreePop();
    }
  }

  void drawScans() {
    CPlayerState *playerState = g_StateManager.GetPlayerState();
    if (!playerState) return;
    auto &scanTimes = playerState->scanStates;
    for (int i = 0; i < scanTimes.length(); i++) {
      auto &scanState = scanTimes[i];
      if (scanState.time > 0) {
        ImGui::Text("%08x: %.2f", scanState.asset, scanState.time);
      }
    }
  }
  } // namespace GUI
