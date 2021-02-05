#ifndef PRIME_PRACTICE_NATIVE_MENU_H
#define PRIME_PRACTICE_NATIVE_MENU_H

#include <include/prime/CFinalInput.hpp>

constexpr int ITEM_HEIGHT = 10;

class Menu {
public:
  virtual void render(int x, int y) const;
  virtual void tick(CFinalInput *inputs);

  void scrollTo(int index);
  void scrollBy(int delta);
  int currentCursor() const;

  virtual Menu *backMenu() const = 0;
  virtual int itemCount() const = 0;
  virtual void renderItem(int index, int x, int y) const = 0;
  virtual void clickItem(int index) = 0;
  virtual int getWidthInCharacters();
private:
  int cursor = 0;
  int scrollTimer = 0;
};

#endif //PRIME_PRACTICE_NATIVE_MENU_H
