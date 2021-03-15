#include <TextRenderer.hpp>
#include <stdio.h>
#include "PlayerMenu.h"
#include "Menus.h"

PlayerMenu MENU_PLAYER{};
bool saved{false};
float savedX{0}, savedY{0}, savedZ{0};


void PlayerMenu::render(int x, int y) const {
  TextRenderer::SetColor(1, 1, 1, 1);
  TextRenderer::RenderText("Player Menu", (float) x, (float) y);

  Menu::render(x, y);
}

int PlayerMenu::itemCount() const {
  return static_cast<int>(PlayerMenuItem::END);
}

void PlayerMenu::renderItem(int index, int x, int y) const {
  auto item = PlayerMenuItem{index};
  switch (item) {
    case PlayerMenuItem::POSITION_INDICATOR:
      if (saved) {
        char buffer[64];
        sprintf(buffer, "Saved pos: %7.3f, %7.3f, %7.3f",
                savedX, savedY, savedZ);
        TextRenderer::RenderText(buffer, x, y);
      } else {
        TextRenderer::RenderText("No saved position", x, y);
      }
      break;
    case PlayerMenuItem::SAVE_POSITION:
      TextRenderer::RenderText("Save Position", x, y);
      break;
    case PlayerMenuItem::LOAD_POSITION:
      TextRenderer::RenderText("Load Position", x, y);
      break;
    case PlayerMenuItem::X:
      break;
    case PlayerMenuItem::Y:
      break;
    case PlayerMenuItem::Z:
      break;
    case PlayerMenuItem::IS_ON:
      break;
    case PlayerMenuItem::LIGHTSHOW_FIX:
      break;
    case PlayerMenuItem::FLOATY_JUMP:
      break;
    case PlayerMenuItem::DISTANCE_UNDER_WATER:
      break;
    case PlayerMenuItem::FLUID_COUNTER:
      break;
    case PlayerMenuItem::JUMP_STATE:
      break;
    case PlayerMenuItem::MORPH_STATE:
      break;
    case PlayerMenuItem::CAMERA_STATE:
      break;
    case PlayerMenuItem::SJ_TIMER:
      break;
    case PlayerMenuItem::FALLING_TIME:
      break;
    default:
    case PlayerMenuItem::END:
      break;
  }
}

void PlayerMenu::clickItem(int index) {
  auto item = PlayerMenuItem{index};
  switch (item) {
    case PlayerMenuItem::POSITION_INDICATOR:
      break; // Do nothing
    case PlayerMenuItem::SAVE_POSITION:
      // TODO
      break;
    case PlayerMenuItem::LOAD_POSITION:
      // TODO
      break;
    case PlayerMenuItem::X:
      // TODO
      break;
    case PlayerMenuItem::Y:
      // TODO
      break;
    case PlayerMenuItem::Z:
      // TODO
      break;
    case PlayerMenuItem::IS_ON:
      // TODO
      break;
    case PlayerMenuItem::LIGHTSHOW_FIX:
      // TODO
      break;
    case PlayerMenuItem::FLOATY_JUMP:
      // TODO
      break;
    case PlayerMenuItem::DISTANCE_UNDER_WATER:
      // TODO
      break;
    case PlayerMenuItem::FLUID_COUNTER:
      // TODO
      break;
    case PlayerMenuItem::JUMP_STATE:
      // TODO
      break;
    case PlayerMenuItem::MORPH_STATE:
      // TODO
      break;
    case PlayerMenuItem::CAMERA_STATE:
      // TODO
      break;
    case PlayerMenuItem::SJ_TIMER:
      // TODO
      break;
    case PlayerMenuItem::FALLING_TIME:
      // TODO
    default:
    case PlayerMenuItem::END:
      break; // do nothing
  }
}

int PlayerMenu::getWidthInCharacters() {
  return 40;
}
