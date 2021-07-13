#ifndef PRIME_PRACTICE_CPLAYER_HPP
#define PRIME_PRACTICE_CPLAYER_HPP

#include "prime/CStateManager.hpp"
#include "prime/CPhysicsActor.hpp"
#include "prime/CCameraBobber.hpp"

class CPlayer : public CPhysicsActor {
public:
	void Teleport(const CTransform4f& newTransform, CStateManager&, bool resetBallCam);


  CCameraBobber *getCameraBobber() { return *GetField<CCameraBobber*>(this, 0x76C); }
};

#endif //PRIME_PRACTICE_CPLAYER_HPP
