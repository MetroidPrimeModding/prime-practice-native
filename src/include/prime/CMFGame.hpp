#pragma once

#include "CInGameGuiManager.hpp"
#include "CStateManager.hpp"

class CMFGame {
public:
  void Draw() const;

  inline CStateManager *getStateManager() { return GetField<CStateManager>(this, 0x14); };
  inline CInGameGuiManager *getInGameGuiManager() { return GetField<CInGameGuiManager>(this, 0x18); };
};
