#pragma once

#include "../PracticeMod.hpp"

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

extern const WarpWorld WARP_WORLD_FRIGATE;
extern const WarpWorld WARP_WORLD_TALLON;
extern const WarpWorld WARP_WORLD_CHOZO;
extern const WarpWorld WARP_WORLD_MAGMOOR;
extern const WarpWorld WARP_WORLD_PHEN;
extern const WarpWorld WARP_WORLD_MINES;
extern const WarpWorld WARP_WORLD_CRATER;
extern const WarpWorld WARP_WORLDS[];

const char* getNameForWorldAsset(u32 world);
const char* getNameForAreaAsset(u32 world, u32 area);

namespace GUI {
  void drawWarpMenu();
}
