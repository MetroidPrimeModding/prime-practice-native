#pragma once
#include "types.h"

class CMapWorldInfo {
public:
  bool IsMapped(TAreaId aid) const;
  bool IsWorldVisible(TAreaId aid) const;
  bool IsAreaVisible(TAreaId aid) const;
  bool IsAnythingSet() const;
};
