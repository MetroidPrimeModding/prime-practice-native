#pragma once

#include "CActor.hpp"
#include "GetField.hpp"
#include "MathyTypes.hpp"
#include "prime/CPhysicsState.hpp"

class CPhysicsActor : public CActor {
public:
  CPhysicsState GetPhysicsState() const;

  inline CVector3f *GetVelocity() { return GetField<CVector3f>(this, 0x138); };
  inline CVector3f *GetAngularVelocity() { return GetField<CVector3f>(this, 0x144); };
};
