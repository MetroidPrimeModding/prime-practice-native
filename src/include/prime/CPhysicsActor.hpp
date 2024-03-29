#pragma once

#include "GetField.hpp"
#include "MathyTypes.hpp"
#include "prime/CPhysicsState.hpp"
#include "CActor.hpp"

class CPhysicsActor : public CActor {
public:
    CPhysicsState GetPhysicsState() const;

    inline CVector3f *GetVelocity() { return GetField<CVector3f>(this, 0x138); };
    inline CVector3f *GetAngularVelocity() { return GetField<CVector3f>(this, 0x144); };
};
