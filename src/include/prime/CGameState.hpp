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

  // CSystemState::mFusionSuitActive: the Extras menu's fusion suit setting, copied into CPlayerState on game start
  inline u64 CardSerial() { return *GetField<u64>(this, 0x210); }

  inline bool HasFusion() { return (*GetField<u8>(this, 0x178) & 0x08) != 0; }
  inline void SetHasFusion(bool v) { *GetField<u8>(this, 0x178) = (*GetField<u8>(this, 0x178) & ~0x08) | (v ? 0x08 : 0); }
};

extern CGameState *gpGameState;
