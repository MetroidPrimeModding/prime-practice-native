#include <settings.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <prime/CStateManager.hpp>
#include <prime/CPlayer.hpp>
#include <prime/CPlayerGun.hpp>
#include <os.h>
#include <NewPauseScreen.hpp>
#include "BombJumping.hpp"

constexpr float FPS = 60;
constexpr float BombChargeTime = 3;
constexpr int FrameDataCount = (BombChargeTime * 2.2f) * FPS;
constexpr int BombMax = 12;

struct BombJumpTimings {
  const char *name;
  int count;
  int timings[BombMax];
};

const BombJumpTimings TIMINGS_UBJ{
    .name="UBJ",
    .count=6,
    .timings={0, 118, 150, 185, 217, 229}
};

const BombJumpTimings TIMINGS_VENT_SHAFT{
    .name="Vent Shaft",
    .count=6,
    .timings={0, 114, 150, 184, 219, 253}
};

BombJumpTimings TIMINGS_CUSTOM{
    .name="Custom",
    .count=6,
//    .timings={0, 50, 75} // dbj (1 - 3 4 method)
  .timings={0, 118, 150, 185, 217, 229} // ubj
};


struct BombJumpFrameData {
  float zPos{0};
};

struct BombJumpUIState {
  int frame = -1;
  BombJumpFrameData frameData[FrameDataCount]{};
  const BombJumpTimings *autoJump = nullptr;

  int currentBomb = 0;
  int bombTimings[BombMax]{0};
  float bombPositions[BombMax]{0};

  bool justRecharged = true;
  int bombRecharges = 0;
  int bombRechargeTiming[BombMax]{0};

  void reset() {
    currentBomb = 0;
    bombRecharges = 0;
    frame = 0;
    autoJump = nullptr;
  }

  void update() {
    if (frame < 0 || frame >= FrameDataCount) return;

    CStateManager *stateManager = CStateManager::instance();
    CPlayer *player = stateManager->Player();
    if (player == nullptr) return;
    CPlayerGun *gun = player->getPlayerGun();
    if (gun == nullptr) return;

    if (gun->x35c_bombTime() <= 0 && !justRecharged) {
      if (bombRecharges < BombMax) {
        bombRechargeTiming[bombRecharges] = frame;
        bombRecharges++;
      }
      justRecharged = true;
    } else if (gun->x35c_bombTime() > 0) {
      justRecharged = false;
    }

    BombJumpFrameData &currentFrame = frameData[frame];
    float zPos = player->getTransform()->z;
    currentFrame.zPos = zPos;

    if (autoJump != nullptr && currentBomb < autoJump->count) {
      if (frame == autoJump->timings[currentBomb]) {
        gun->DropBomb(EBWeapon::Bomb, *stateManager);
//        player->getPlayerGun()->DropBomb(EBWeapon::PowerBomb, *stateManager);
        bombDropped();
      }
    } else {
      autoJump = nullptr;
    }

    frame++;
  }

  void bombDropped() {
    if (frame < 0 || frame >= FrameDataCount) {
      reset();
    }
    if (currentBomb < BombMax) {
      CStateManager *stateManager = CStateManager::instance();
      CPlayer *player = stateManager->Player();

      bombTimings[currentBomb] = frame;
      bombPositions[currentBomb] = player->getTransform()->z;

      currentBomb++;
    }
  }
};

struct FloatStats {
  float min{FLT_MAX};
  float max{-FLT_MAX};
  float avg{0};
  float total{0};
};

FloatStats calcStats(float (*values_getter)(int idx), int start, int end) {
  FloatStats res;
  for (int i = start; i < end; i++) {
    float f = values_getter(i);
    res.total += f;
    if (res.max < f) res.max = f;
    if (res.min > f) res.min = f;
  }
  res.avg = res.total / (float) (end - start);
  return res;
}

BombJumpUIState *state;

void init() {
  if (state == nullptr) {
    state = new BombJumpUIState();
  }
}

void deinit() {
  if (state != nullptr) {
    delete state;
    state = nullptr;
  }
}

void GUI::drawBombJumpingInterface() {
  if (!SETTINGS.BOMBJUMP_enable) {
    deinit();
    return;
  }
  init();

  auto *inputs = NewPauseScreen::instance->inputs;

  if (inputs[1].PA() && state->autoJump == nullptr) {
    state->reset();
    state->autoJump = &TIMINGS_UBJ;
  }
  if (inputs[1].PX() && state->autoJump == nullptr) {
    state->reset();
    state->autoJump = &TIMINGS_VENT_SHAFT;
  }
  if (inputs[1].PB() && state->autoJump == nullptr) {
    state->reset();
    state->autoJump = &TIMINGS_CUSTOM;
  }

  state->update();

  CStateManager *stateManager = CStateManager::instance();
  CPlayer *player = stateManager->Player();
  if (player == nullptr) return;
  CPlayerGun *gun = player->getPlayerGun();
  if (gun == nullptr) return;

  {
    ImGui::SetNextWindowPos(ImVec2(10, 450), ImGuiCond_None, ImVec2(0, 1));
    ImGui::Begin(
        "Bomb Jumping", nullptr,
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

    auto posStats = calcStats([](int idx) { return state->frameData[idx].zPos; }, 0, state->frame);

    float jumpHeight = posStats.max - posStats.min;
    if (posStats.max - posStats.min < 7) {
      posStats.max = posStats.min + 7;
    }

    constexpr float graphWidth = 200.0f;
    constexpr float graphHeight = 50.0f;
    ImVec2 graphPadding = ImGui::GetStyle().FramePadding;
    float posScale = graphHeight / (float) (posStats.max - posStats.min);

    ImDrawList *dl = ImGui::GetWindowDrawList();

    // recharge
    {
      float recharge = gun->x35c_bombTime();
      float rechargePercent = 1 - (recharge / 3.0f);
      if (rechargePercent > 1) rechargePercent = 1;
      if (rechargePercent < 0) rechargePercent = 0;
      if (recharge < 0) recharge = 0;
      char buf[32];
      snprintf(buf, sizeof(buf), "%0.2f", recharge);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::ProgressBar(rechargePercent, ImVec2(-1.0f, 8.0f), "");
      dl->AddText(p + ImVec2(graphWidth / 2 - 15, 0), IM_COL32(255, 255, 255, 255), buf);
    }

    ImVec2 graphBottomLeft = ImGui::GetCursorScreenPos() + ImVec2(0, graphHeight - graphPadding.y);
    if (state->autoJump != nullptr) {
      ImGui::Text("Auto: %s", state->autoJump->name);
    }

    // Bomb recharges
    for (int i = 0; i < state->bombRecharges; i++) {
      float percent = (float) state->bombRechargeTiming[i] / (float) state->frame;
      float x = (graphWidth - graphPadding.x * 2) * percent + graphPadding.x;
      dl->AddLine(graphBottomLeft + ImVec2(x, graphPadding.y), graphBottomLeft + ImVec2(x, graphPadding.y - graphHeight),
                  IM_COL32(200, 200, 200, 100));
    }

    // backdrop
    dl->AddRectFilled(
        ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImVec2(graphWidth, graphHeight),
        ImGui::GetColorU32(ImGuiCol_FrameBg)
    );

    // Bomb boxes
    for (int i = 0; i < state->currentBomb; i++) {
      float hitYMin = state->bombPositions[i] - 0.7f; // - half radius - bomb jump height
      float hitYMax = state->bombPositions[i] + 1.5f;// - half radius + bomb jump height

//      float yOffset = (float)yPosOffset * -posScale;
      float yMin = (hitYMin - posStats.min) * -posScale;
      float yMax = (hitYMax - posStats.min) * -posScale;

      float dropPercent = (float) (state->bombTimings[i] + 1) / (float) state->frame;
      float dropX = (graphWidth - graphPadding.x * 2) * dropPercent + graphPadding.x;

      ImVec2 calculatedDropStart = graphBottomLeft + ImVec2(dropX, yMin);
      ImVec2 calculatedDropEnd = graphBottomLeft + ImVec2(dropX, yMax);

      dl->AddLine(calculatedDropStart, calculatedDropEnd, IM_COL32(255, 255, 0, 255));

      float explodePercent = (float) (state->bombTimings[i] + 62) / (float) state->frame;
      float explodeX = (graphWidth - graphPadding.x * 2) * explodePercent + graphPadding.x;
      ImVec2 calculatedExplodeStart = graphBottomLeft + ImVec2(explodeX, yMin);
      ImVec2 calculatedExplodeEnd = graphBottomLeft + ImVec2(explodeX, yMax);

      dl->AddLine(calculatedExplodeStart, calculatedExplodeEnd, IM_COL32(255, 0, 0, 255));

      dl->AddLine(calculatedDropStart, calculatedExplodeStart, IM_COL32(127, 127, 127, 127));
      dl->AddLine(calculatedDropEnd, calculatedExplodeEnd, IM_COL32(127, 127, 127, 127));
    }

    // pos plot
    ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);
    ImGui::PlotLines(
        "",
        [](void *data, int idx) { return state->frameData[idx].zPos; },
        nullptr, state->frame, 0,
        "",
        posStats.min, posStats.max,
        ImVec2(graphWidth, graphHeight)
    );
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = color;

    ImGui::Text("Height: %0.02f", jumpHeight);
    ImGui::SameLine();
    ImGui::Text("Bomb Timings:");
    for (int i = 0; i < state->currentBomb; i++) {
      if (i > 0) {
        ImGui::SameLine();
      }
      ImGui::Text("%d", state->bombTimings[i]);
    }
    for (int i = 0; i < state->currentBomb; i++) {
      if (i > 0) {
        ImGui::SameLine();
        ImGui::Text("%d", state->bombTimings[i] - state->bombTimings[i - 1]);
      } else {
        ImGui::Text("/");
      }
    }

    ImGui::End();
  }
}

void GUI::bombDropped() {
  if (state != nullptr) {
    state->bombDropped();
  }
}

void GUI::drawCustomBombJumpConfigMenu() {
  ImGui::DragInt("Count", &TIMINGS_CUSTOM.count, 1.0f, 1, BombMax);
  char label[32];
  char fmt[32];
  for (int i = 0; i < TIMINGS_CUSTOM.count; i++) {
    int last = 0;
    if (i > 0) {
      last = TIMINGS_CUSTOM.timings[i - 1];
    }
    snprintf(label, sizeof(fmt), "%d", i);
    snprintf(fmt, sizeof(fmt), "%d/%+d", TIMINGS_CUSTOM.timings[i], TIMINGS_CUSTOM.timings[i] - last);
    ImGui::DragInt(label, &TIMINGS_CUSTOM.timings[i], 1.0f, 0, 10000, fmt);
  }
}
