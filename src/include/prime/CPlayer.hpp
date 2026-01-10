#pragma once

#include "prime/CPhysicsActor.hpp"
#include "prime/CStateManager.hpp"

class CPlayerGun;
class CCameraBobber;

class CPlayer : public CPhysicsActor {
public:
  enum class EPlayerMovementState: u32 {
    OnGround=0, StartJump, InAir, Falling, FallingMorphed
  };


  void Teleport(const CTransform4f &newTransform, CStateManager &, bool resetBallCam);

  CCameraBobber *getCameraBobber() { return *GetField<CCameraBobber *>(this, 0x76C); }
  CPlayerGun *getPlayerGun() { return *GetField<CPlayerGun *>(this, 0x490); }
  float *getDepthUnderWater() { return GetField<float>(this, 0x828); }
  TUniqueId getOrbitTargetId() { return *GetField<TUniqueId>(this, 0x310); }
  TUniqueId getScanningObjectId() { return *GetField<TUniqueId>(this, 0x3B4); }
  float *getScanningTime() { return GetField<float>(this, 0x3AC); }
  float *getCurScanTime() { return GetField<float>(this, 0x3B0); }
  EPlayerMovementState *getMovementState() { return GetField<EPlayerMovementState>(this, 0x258); }
  float *getDashButtonHoldTime() { return GetField<float>(this, 0x388); }
};
