#include "EditValueMenu.h"

#include "Menus.h"
#include "NewPauseScreen.hpp"

constexpr EditValueMenu::EditValueMenu() = default;

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
