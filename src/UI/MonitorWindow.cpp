#include "os.h"
#include <stdio.h>
#include <imgui.h>
#include <prime/CGameAllocator.hpp>
#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CGameGlobalObjects.hpp>
#include <prime/CGameState.hpp>
#include <prime/CWorld.hpp>
#include "MonitorWindow.hpp"
#include "settings.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_internal.h"

namespace GUI {
  void drawFrameTime();
  void drawMemoryUsage();
  void drawInput(CFinalInput *inputs);
  void drawPos();
  void drawSpeed();

  void drawIGT();

  void drawRoomTime();

  void drawMonitorWindow(CFinalInput *inputs) {
    if (!SETTINGS.OSD_show) {
      return;
    }
    ImGui::SetNextWindowPos(ImVec2(620, 20), ImGuiCond_None, ImVec2(1, 0));
    ImGui::Begin(
        "Montitor", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNavInputs |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoDecoration |
//        ImGuiWindowFlags_NoBackground |
        ImGuiFocusedFlags_None // just for conveneint commenting in/out
    );
    if (SETTINGS.OSD_showIGT) {
      drawIGT();
    }
    if (SETTINGS.OSD_showRoomTime) {
      if (SETTINGS.OSD_showIGT) {
        ImGui::SameLine();
      }
      drawRoomTime();
    }
    if (SETTINGS.OSD_showPos) {
      drawPos();
    }
    if (SETTINGS.OSD_showVelocity) {
      drawSpeed();
    }
    if (SETTINGS.OSD_showFrameTime) {
      drawFrameTime();
    }
    if (SETTINGS.OSD_showMemoryGraph || SETTINGS.OSD_showMemoryInfo) {
      drawMemoryUsage();
    }
    if (SETTINGS.OSD_showInput) {
      drawInput(inputs);
    }

    ImGui::End();
  }

  void drawIGT() {
    CGameGlobalObjects *globals = ((CGameGlobalObjects *) 0x80457798);
    CGameState *gameState = globals->x134_gameState;
    if (gameState) {
      double time = gameState->PlayTime();
      int ms = (int)(time * 1000.0) % 1000;
      int seconds = (int)time % 60;
      int minutes = ((int)time / 60) % 60;
      int hours = ((int)time / 60 / 60) % 60;
      ImGui::Text("%02d:%02d:%02d.%03d", hours, minutes, seconds, ms);
    }
  }

  u32 last_room = -1;
  double last_time = 0;
  double room_start_time = 0;
  void drawRoomTime() {
    CGameGlobalObjects *globals = ((CGameGlobalObjects *) 0x80457798);
    CGameState *gameState = globals->x134_gameState;
    CStateManager *stateManager = ((CStateManager *) 0x8045A1A8);
    const CWorld *world = stateManager->GetWorld();

    if (gameState && world) {
      u32 current_room = world->GetCurrentAreaId();
      double current_time = gameState->PlayTime();

      if (current_room != last_room) {
        last_time = current_time - room_start_time;
        room_start_time = current_time;
        last_room = current_room;
      }
      double current_room_time = current_time - room_start_time;
      {
        int frames = (int)(last_time / (1.0/60.0));
        int ms = (int) (last_time * 1000.0) % 1000;
        int seconds = (int) last_time % 60;
        int minutes = ((int) last_time / 60) % 60;
        int hours = ((int) last_time / 60 / 60) % 60;
        ImGui::Text("Last: %02d:%02d:%02d.%03d|%d", hours, minutes, seconds, ms, frames);
      }
      {
        int frames = (int)(current_room_time / (1.0/60.0));
        int ms = (int) (current_room_time * 1000.0) % 1000;
        int seconds = (int) current_room_time % 60;
        int minutes = ((int) current_room_time / 60) % 60;
        int hours = ((int) current_room_time / 60 / 60) % 60;
        ImGui::SameLine();
        ImGui::Text("Current: %02d:%02d:%02d.%03d|%d", hours, minutes, seconds, ms, frames);
      }
    }
  }

  void drawPos() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();

    if (player) {
      float x = player->getTransform()->matrix[3];
      float y = player->getTransform()->matrix[7];
      float z = player->getTransform()->matrix[11];
      ImGui::Text("Pos: %7.2fx %7.2fy %7.2fz", x, y, z);
    }
  }

  void drawSpeed() {
    CStateManager *stateManager = CStateManager_INSTANCE;
    CPlayer *player = stateManager->Player();

    if (player) {
      float x = player->GetVelocity()->x;
      float y = player->GetVelocity()->y;
      float z = player->GetVelocity()->z;
      float h = CMath::SqrtF(x * x + y * y);

      ImGui::Text("Vel: %5.2fx %5.2fy %5.2fz %5.2fh", x, y, z, h);
    }
  }

  s64 lastFrame;
  constexpr u32 GRAPH_LENGTH = 32;
  float frames[GRAPH_LENGTH];

  void drawFrameTime() {
    s64 thisFrame = OSGetTime();
    s64 diff = thisFrame - lastFrame;
    lastFrame = thisFrame;

    float ms = static_cast<float>(diff) / static_cast<float>(TICKS_PER_MS);
    for (int i = 1; i < GRAPH_LENGTH; i++) {
      frames[i - 1] = frames[i];
    }
    frames[GRAPH_LENGTH - 1] = ms;

    char title[32];
    snprintf(title, sizeof(title), "Frame time: %02.2f", ms);
    ImGui::PlotLines(
        "",
        frames, GRAPH_LENGTH, 0,
        title,
        0.f, 32.f,
        ImVec2(0, 40.0f)
    );
  }

  float memoryUsage[GRAPH_LENGTH];
  u32 minFree = UINT32_MAX;
  float peakPercent = 0;
  constexpr u32 memoryGraphUpdateRate = 30;
  u32 memoryGraphUpdate = 0;

  void drawMemoryUsage() {
    CGameAllocator *allocator = CGameAllocator_LOCATION;

    u32 freeCount = 0;
    u32 freeSize = 0;
    u32 usedCount = 0;
    u32 usedSize = 0;


    for (CMemoryBlock *block = allocator->first(); block != nullptr; block = block->next) {
      if (freeCount + usedCount > 10000 || !VALID_PTR(block)) {
        break; // Don't infinite loop
      }
      if (block->dataStart == 0) {
        freeCount++;
        freeSize += block->size + 32;
      } else {
        usedCount++;
        usedSize += block->size + 32;
      }
    }

    u32 totalHeapSize = usedSize + freeSize;

    memoryGraphUpdate++;
    if (memoryGraphUpdate > memoryGraphUpdateRate) {
      memoryGraphUpdate = 0;
      for (int i = 1; i < GRAPH_LENGTH; i++) {
        memoryUsage[i - 1] = memoryUsage[i];
      }
      memoryUsage[GRAPH_LENGTH - 1] = usedSize;
    }

    u32 freePrecent = freeSize * 100 / totalHeapSize;
    u32 usedPercent = 100 - freePrecent;


    if (freeSize < minFree) {
      minFree = freeSize;
    }

    if (usedPercent > peakPercent) {
      peakPercent = usedPercent;
    }

    if (SETTINGS.OSD_showMemoryInfo) {
      ImGui::Text("Used: %d", usedSize);
      ImGui::Text("Free: %d", freeSize);
      ImGui::Text("Total: %d", totalHeapSize);
    }

    if (SETTINGS.OSD_showMemoryGraph) {
      char title[32];
      snprintf(title, sizeof(title), "%d%% used, %d%% free", usedPercent, freePrecent);
      ImGui::PlotLines(
          "",
          memoryUsage, GRAPH_LENGTH, 0,
          title,
          0.f, totalHeapSize,
          ImVec2(0, 40.0f)
      );
    }
  }


  void drawInput(CFinalInput *inputs) {
    CFinalInput *p1 = &inputs[0];

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    char text[64];
    int len = 0;

    constexpr float leftStickRadius = 15;
    ImVec2 leftStickCenter = p + ImVec2(15, 35);
    constexpr float dpadRadius = 10;
    constexpr float dpadWidth = 4;
    ImVec2 dpadCenter = p + ImVec2(40, 60);
    constexpr float rightStickRadius = 10;
    ImVec2 rightStickCenter = p + ImVec2(80, 60);
    constexpr float startButtonRadius = 4;
    ImVec2 startButtonCenter = p + ImVec2(60, 40);
    constexpr float aButtonRadius = 8;
    ImVec2 aButtonCenter = p + ImVec2(110, 38);
    constexpr float bButtonRadius = 4;
    ImVec2 bButtonCenter = aButtonCenter + ImVec2(-12, 8);
    constexpr float xButtonRadius = 4;
    ImVec2 xButtonCenter = aButtonCenter + ImVec2(12, -8);
    constexpr float yButtonRadius = 4;
    ImVec2 yButtonCenter = aButtonCenter + ImVec2(-6, -12);
    constexpr float triggerWidth = leftStickRadius * 2;
    constexpr float triggerHeight = 4;
    ImVec2 lCenter = leftStickCenter + ImVec2(0, -25);
    ImVec2 rCenter = ImVec2(aButtonCenter.x, lCenter.y);


    constexpr ImU32 stickGray = IM_COL32(150, 150, 150, 255);
    constexpr ImU32 darkGray = IM_COL32(60, 60, 60, 255);
    constexpr ImU32 red = IM_COL32(255, 0, 0, 255);
    constexpr ImU32 green = IM_COL32(0, 255, 0, 255);

    // left stick
    {
      dl->AddCircleFilled(
          leftStickCenter, leftStickRadius,
          stickGray,
          8
      );
      float x = p1->ALeftX();
      float y = p1->ALeftY();
      dl->AddLine(
          leftStickCenter,
          leftStickCenter + ImVec2(x * leftStickRadius, y * leftStickRadius),
          red
      );
    }

    // right stick
    {
      dl->AddCircleFilled(
          rightStickCenter, rightStickRadius,
          stickGray,
          8
      );
      float x = p1->ARightX();
      float y = p1->ARightY();
      dl->AddLine(
          rightStickCenter,
          rightStickCenter + ImVec2(x * rightStickRadius, y * rightStickRadius),
          red
      );
    }

    // dpad
    {
      constexpr float halfWidth = dpadWidth / 2;
      dl->AddRectFilled(
          dpadCenter + ImVec2(-halfWidth, -dpadRadius),
          dpadCenter + ImVec2(halfWidth, dpadRadius),
          stickGray
      );

      dl->AddRectFilled(
          dpadCenter + ImVec2(-dpadRadius, -halfWidth),
          dpadCenter + ImVec2(dpadRadius, halfWidth),
          stickGray
      );

      if (p1->DDPUp()) {
        dl->AddRectFilled(
            dpadCenter + ImVec2(-halfWidth, -dpadRadius),
            dpadCenter + ImVec2(halfWidth, -dpadRadius / 2),
            red
        );
      }

      if (p1->DDPDown()) {
        dl->AddRectFilled(
            dpadCenter + ImVec2(-halfWidth, dpadRadius),
            dpadCenter + ImVec2(halfWidth, dpadRadius / 2),
            red
        );
      }

      if (p1->DDPLeft()) {
        dl->AddRectFilled(
            dpadCenter + ImVec2(-dpadRadius, -halfWidth),
            dpadCenter + ImVec2(-dpadRadius / 2, halfWidth),
            red
        );
      }

      if (p1->DDPRight()) {
        dl->AddRectFilled(
            dpadCenter + ImVec2(dpadRadius, -halfWidth),
            dpadCenter + ImVec2(dpadRadius / 2, halfWidth),
            red
        );
      }
    }

    // buttons
    {
      // start
      dl->AddCircleFilled(
          startButtonCenter, startButtonRadius,
          p1->DStart() ? red : stickGray,
          16
      );

      // a
      dl->AddCircleFilled(
          aButtonCenter, aButtonRadius,
          p1->DA() ? green : stickGray,
          16
      );

      //b
      dl->AddCircleFilled(
          bButtonCenter, bButtonRadius,
          p1->DB() ? red : stickGray,
          16
      );

      //x
      dl->AddCircleFilled(
          xButtonCenter, xButtonRadius,
          p1->DX() ? red : stickGray,
          16
      );

      //y
      dl->AddCircleFilled(
          yButtonCenter, yButtonRadius,
          p1->DY() ? red : stickGray,
          16
      );
    }

    // triggers
    {
      float halfTriggerWidth = triggerWidth / 2;
      ImVec2 lStart = lCenter - ImVec2(halfTriggerWidth, 0);
      ImVec2 lEnd = lCenter + ImVec2(halfTriggerWidth, triggerHeight);
      float lValue = triggerWidth * p1->ALTrigger();

      dl->AddRectFilled(
          lStart, lStart + ImVec2(lValue, triggerHeight),
          p1->DL() ? red : stickGray
      );
      dl->AddRectFilled(
          lStart + ImVec2(lValue, 0), lEnd,
          darkGray
      );

      ImVec2 rStart = rCenter - ImVec2(halfTriggerWidth, 0);
      ImVec2 rEnd = rCenter + ImVec2(halfTriggerWidth, triggerHeight);
      float rValue = triggerWidth * p1->ARTrigger();

      dl->AddRectFilled(
          rEnd - ImVec2(rValue, triggerHeight), rEnd,
          p1->DR() ? red : stickGray
      );
      dl->AddRectFilled(
          rStart, rEnd - ImVec2(rValue, 0),
          darkGray
      );
    }

    ImGui::Dummy(ImVec2(130, 80));
  }

}
