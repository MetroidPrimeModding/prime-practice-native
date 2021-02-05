#include "EditValueMenu.h"

#include "Menus.h"
#include "include/NewPauseScreen.hpp"

constexpr EditValueMenu::EditValueMenu() = default;

Menu *EditValueMenu::backMenu() const {
  if (this->lastMenu != nullptr) {
    return this->lastMenu;
  }
  return &MENU_MAIN;
}

void EditValueMenu::clickItem(int index) {
  // Do nothing, we override tick()
}

void EditValueMenu::tick(CFinalInput *inputs) {

  // we do NOT call the original here, so we have to handle back ourselves
  if (inputs->PB()) {
    NewPauseScreen::instance->popMenu();
    return;
  }
}
