//
// Created by Christopher Freestone on 1/30/21.
//

#include <include/TextRenderer.hpp>
#include "MainMenu.h"
#include "Menus.h"

MainMenu MENU_MAIN;

void MainMenu::render(int x, int y) const {
  TextRenderer::SetColor(1, 1, 1, 1);
  TextRenderer::RenderText("Main menu", (float) x, (float) y);

  Menu::render(x, y);
}

int MainMenu::itemCount() const {
  return static_cast<int>(MainMenuItem::END);
}

void MainMenu::renderItem(int index, int x, int y) const {
  if (this->currentCursor() == index) {
    TextRenderer::SetColor(1, 1, 1, 1);
  } else {
    TextRenderer::SetColor(0.4, 0.4, 0.4, 1);
  }
  auto item = MainMenuItem{index};
  switch (item) {
    case MainMenuItem::INVENTORY:
      TextRenderer::RenderText("Inventory", x, y);
      break;
    case MainMenuItem::PLAYER:
      TextRenderer::RenderText("Player", x, y);
      break;
    case MainMenuItem::WARP:
      TextRenderer::RenderText("Warp", x, y);
      break;
    case MainMenuItem::CONFIG:
      TextRenderer::RenderText("Config", x, y);
      break;
    default:
      break;
  }
}

Menu * MainMenu::clickItem(int index) {
  auto item = MainMenuItem{index};
  switch (item) {
    case MainMenuItem::INVENTORY:
      return nullptr;
    case MainMenuItem::PLAYER:
      return nullptr;
    case MainMenuItem::WARP:
      return &MENU_WARP_MAIN;
    case MainMenuItem::CONFIG:
      return nullptr;
    default:
    case MainMenuItem::END:
      return nullptr;
  }
}

Menu *MainMenu::backMenu() const {
  return &MENU_MAIN;
}
