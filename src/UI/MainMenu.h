//
// Created by Christopher Freestone on 1/30/21.
//

#ifndef PRIME_PRACTICE_NATIVE_MAINMENU_H
#define PRIME_PRACTICE_NATIVE_MAINMENU_H

#include "Menu.h"

enum class MainMenuItem : int {
  INVENTORY = 0,
  PLAYER,
  WARP,
  CONFIG,
  END
};


class MainMenu : public Menu {
public:
  void render(int x, int y) const override;

  [[nodiscard]] Menu *backMenu() const override;
  [[nodiscard]] int itemCount() const override;
  void renderItem(int index, int x, int y) const override;
  Menu *clickItem(int index) override;
};

#endif //PRIME_PRACTICE_NATIVE_MAINMENU_H
