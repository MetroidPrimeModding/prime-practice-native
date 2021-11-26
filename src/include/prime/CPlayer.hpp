#ifndef PRIME_PRACTICE_CPLAYER_HPP
#define PRIME_PRACTICE_CPLAYER_HPP

#include "prime/CStateManager.hpp"
#include "prime/CPhysicsActor.hpp"

class CPlayerGun;
class CCameraBobber;

class CPlayer : public CPhysicsActor {
public:
	void Teleport(const CTransform4f& newTransform, CStateManager&, bool resetBallCam);


  CCameraBobber *getCameraBobber() { return *GetField<CCameraBobber*>(this, 0x76C); }
  CPlayerGun *getPlayerGun() { return *GetField<CPlayerGun*>(this, 0x490); }
  float *getDepthUnderWater() { return GetField<float>(this, 0x828); }
};

#endif //PRIME_PRACTICE_CPLAYER_HPP
