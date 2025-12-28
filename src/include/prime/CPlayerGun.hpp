#pragma once

#include "CStateManager.hpp"
#include "PrimeAPI.h"

enum class EBWeapon { Bomb, PowerBomb };
enum class EChargePhase {
  NotCharging,
  ChargeRequested,
  AnimAndSfx,
  FxGrowing,
  FxGrown,
  ComboXfer,
  ComboXferDone,
  ComboFire,
  ComboFireDone,
  ChargeCooldown,
  ChargeDone
};

class CPlayerGun {
public:
  void DropBomb(EBWeapon weapon, CStateManager &mgr);
  void ChangeWeapon(const CPlayerState &rPlayerState, CStateManager &rStateMgr);
  void StopChargeSound(CStateManager &rStateMgr);
  void DropPowerBomb(CStateManager &rStateMgr) const;
  void FireSecondary(CStateManager &rStateMgr);

  EChargePhase x32c_chargePhase() { return *GetField<EChargePhase>(this, 0x32c); }
  u32 *x308_bombCount() { return GetField<u32>(this, 0x308); }
  float x35c_bombTime() { return *GetField<float>(this, 0x35C); }
};
