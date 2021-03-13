#include <include/TextRenderer.hpp>
#include <include/NewPauseScreen.hpp>
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

void MainMenu::clickItem(int index) {
  auto item = MainMenuItem{index};
  switch (item) {
    case MainMenuItem::INVENTORY:
      // TODO
      break;
    case MainMenuItem::PLAYER:
      NewPauseScreen::instance->pushMenu(&MENU_PLAYER);
      break;
    case MainMenuItem::WARP:
      NewPauseScreen::instance->pushMenu(&MENU_WARP_MAIN);
      break;
    case MainMenuItem::CONFIG:
      // todo
      break;
    default:
    case MainMenuItem::END:
      break; // do nothing
  }
}

Menu *MainMenu::backMenu() const {
  return &MENU_MAIN;
}

int MainMenu::getWidthInCharacters() {
  return 24;
}
