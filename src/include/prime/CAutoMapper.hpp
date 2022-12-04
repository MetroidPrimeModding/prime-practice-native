#pragma once

#include "../GetField.hpp"
#include "CWorld.hpp"

class CAutoMapper {
public:

  void ProcessMapScreenInput(const CFinalInput& input, CStateManager& mgr);

  inline IWorld *world() { return *GetField<IWorld*>(this, 0x24); }
  inline TAreaId *curAreaId() { return GetField<TAreaId>(this, 0xA0); }
};
