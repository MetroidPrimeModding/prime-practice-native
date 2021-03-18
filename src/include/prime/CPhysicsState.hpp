#ifndef PRIME_PRACTICE_CPHYSICSSTATE_HPP
#define PRIME_PRACTICE_CPHYSICSSTATE_HPP

#include "MathyTypes.hpp"

class CPhysicsState {
public:
    CVector3f x0_translation;
    CQuaternion xc_orientation;
    CVector3f x1c_constantForce;
    CAxisAngle x28_angularMomentum;
    CVector3f x34_momentum;
    CVector3f x40_force;
    CVector3f x4c_impulse;
    CAxisAngle x58_torque;
    CAxisAngle x64_angularImpulse;

    CPhysicsState(const CVector3f &translation, const CQuaternion &orient,
                  const CVector3f &v2, const CAxisAngle &a1, const CVector3f &v3,
                  const CVector3f &v4, const CVector3f &v5, const CAxisAngle &a2,
                  const CAxisAngle &a3): x0_translation(translation)
        , xc_orientation(orient)
        , x1c_constantForce(v2)
        , x28_angularMomentum(a1)
        , x34_momentum(v3)
        , x40_force(v4)
        , x4c_impulse(v5)
        , x58_torque(a2)
        , x64_angularImpulse(a3)
    {};

};

#endif //PRIME_PRACTICE_CPHYSICSSTATE_HPP
