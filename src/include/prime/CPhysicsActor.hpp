#ifndef PRIME_PRACTICE_CPHYSICSACTOR_HPP
#define PRIME_PRACTICE_CPHYSICSACTOR_HPP

#include "include/GetField.hpp"
#include "include/MathyTypes.hpp"
#include "include/prime/CPhysicsState.hpp"
#include "CActor.hpp"

class CPhysicsActor : public CActor {
public:
    CPhysicsState GetPhysicsState() const;

    CVector3f *GetVelocity() { return GetField<CVector3f>(this, 0x138); };
    CVector3f *GetAngularVelocity() { return GetField<CVector3f>(this, 0x144); };
};

#endif //PRIME_PRACTICE_CPHYSICSACTOR_HPP
