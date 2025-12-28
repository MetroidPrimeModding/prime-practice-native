#pragma once

#include <GetField.hpp>

class CWorldState;
class CCameraManager;

class CGameState {
public:
  void SetCurrentWorldId(CAssetId id);
  CWorldState &StateForWorld(unsigned int world);
  CWorldState &CurrentWorldState();

  inline CAssetId MLVL() { return *GetField<u32>(this, 0x84); };
  inline double PlayTime() { return *GetField<double>(this, 0xa0); }
};

extern CGameState *g_GameState;
