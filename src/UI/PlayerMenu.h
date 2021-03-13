#ifndef PRIME_PRACTICE_NATIVE_PLAYERMENU_H
#define PRIME_PRACTICE_NATIVE_PLAYERMENU_H

#include "Menu.h"

enum class PlayerMenuItem : int {
  POSITION_INDICATOR,
  SAVE_POSITION,
  LOAD_POSITION,
  X,
  Y,
  Z,
  IS_ON,
  LIGHTSHOW_FIX,
  FLOATY_JUMP,
  DISTANCE_UNDER_WATER,
  FLUID_COUNTER,
  JUMP_STATE,
  MORPH_STATE,
  CAMERA_STATE,
  SJ_TIMER,
  FALLING_TIME,
  END
};

class PlayerMenu : public Menu {
public:
  void render(int x, int y) const override;

  [[nodiscard]] int itemCount() const override;
  void renderItem(int index, int x, int y) const override;
  void clickItem(int index) override;
  int getWidthInCharacters() override;
};


#endif //PRIME_PRACTICE_NATIVE_PLAYERMENU_H
