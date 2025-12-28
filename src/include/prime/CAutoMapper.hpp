#pragma once

#include "../GetField.hpp"
#include "CWorld.hpp"

enum class EAutoMapperState { MiniMap, MapScreen, MapScreenUniverse };

class CAutoMapper {
public:
  void ProcessMapScreenInput(const CFinalInput &input, CStateManager &mgr);

  inline EAutoMapperState state() { return *GetField<EAutoMapperState>(this, 0x1bc); }
  inline EAutoMapperState nextState() { return *GetField<EAutoMapperState>(this, 0x1c0); }
  inline IWorld *world() { return *GetField<IWorld *>(this, 0x24); }
  inline TAreaId *curAreaId() { return GetField<TAreaId>(this, 0xA0); }

  void Draw(const CStateManager &manager, const CTransform4f &f, float d);
};
