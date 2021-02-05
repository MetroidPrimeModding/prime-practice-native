#ifndef PRIME_PRACTICE_NATIVE_EDITVALUEMENU_H
#define PRIME_PRACTICE_NATIVE_EDITVALUEMENU_H

#include "Menu.h"

class EditValueMenu: public Menu {
public:
  constexpr EditValueMenu();
  void render(int x, int y) const override;

  [[nodiscard]] Menu *backMenu() const override;
  void clickItem(int index) override;
  void tick(CFinalInput *inputs) override;

  // Handle value changes
  virtual void click() = 0;
  virtual void leftMax() = 0;
  virtual void leftFast() = 0;
  virtual void leftSlow() = 0;
  virtual void rightSlow() = 0;
  virtual void rightFast() = 0;
  virtual void rightMax() = 0;


  // Mode settings, public
  const char *valueName{""};
  Menu *lastMenu{nullptr};
};


#endif //PRIME_PRACTICE_NATIVE_EDITVALUEMENU_H
