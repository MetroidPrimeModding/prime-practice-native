#pragma once

#include "CHealthInfo.hpp"
#include "CPhysicsActor.hpp"

class CAi : public CPhysicsActor {
public:
  inline CHealthInfo *GetHealthInfo() { return GetField<CHealthInfo>(this, 0x258); };
};
