#ifndef PRIME_PRACTICE_NATIVE_WARPMENU_H
#define PRIME_PRACTICE_NATIVE_WARPMENU_H

#include "Menu.h"

enum class WarpMainMenuItem {
  FRIGATE,
  TALLON,
  CHOZO,
  MAGMOOR,
  PHEN,
  MINES,
  CRATER,
  END
};

class WarpMainMenu: public Menu {
public:
  void render(int x, int y) const override;
  [[nodiscard]] int itemCount() const override;
  void renderItem(int index, int x, int y) const override;
  void clickItem(int index) override;
};

struct WarpArea {
  const char* name;
  u32 area;
};

struct WarpWorld {
  const char* name;
  u32 world;
  const WarpArea* areas;
  int areaCount;
};

class WarpMenu: public Menu {
public:
  explicit constexpr WarpMenu(const WarpWorld *world);
  void render(int x, int y) const override;

  [[nodiscard]] int itemCount() const override;
  void renderItem(int index, int x, int y) const override;
  void clickItem(int index) override;
  int getWidthInCharacters() override;

private:
  const WarpWorld *world;
};

extern const WarpWorld WARP_WORLD_FRIGATE;
extern const WarpWorld WARP_WORLD_TALLON;
extern const WarpWorld WARP_WORLD_CHOZO;
extern const WarpWorld WARP_WORLD_MAGMOOR;
extern const WarpWorld WARP_WORLD_PHEN;
extern const WarpWorld WARP_WORLD_MINES;
extern const WarpWorld WARP_WORLD_CRATER;

#endif //PRIME_PRACTICE_NATIVE_WARPMENU_H
