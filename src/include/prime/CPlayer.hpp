#ifndef PRIME_PRACTICE_CPLAYER_HPP
#define PRIME_PRACTICE_CPLAYER_HPP

#include "prime/CPhysicsActor.hpp"

class CPlayer : public CPhysicsActor {
public:
	void Teleport(const CTransform4f& newTransform, CStateManager&, bool resetBallCam);
};

#endif //PRIME_PRACTICE_CPLAYER_HPP
