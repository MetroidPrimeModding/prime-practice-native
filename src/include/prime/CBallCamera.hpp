#pragma once

#include "CGameCamera.hpp"

class CBallCamera : public CGameCamera {
public:
  bool x18d_24_prevClearLOS() { return (*GetField<u8>(this, 0x18d) >> 7) & 1; }
  bool x18c_31_clearLOS() { return (*GetField<u8>(this, 0x18c) >> 7) & 1; }
  bool x18c_27_obscureAvoidance() { return (*GetField<u8>(this, 0x18c) >> 3) & 1; }
  bool x18e_27_nearbyDoorClosed() { return (*GetField<u8>(this, 0x18e) >> 3) & 1; }

  u64 x350_obscuringMaterial() { return *GetField<u64>(this, 0x350); }
  float x34c_obscuredTime() { return *GetField<float>(this, 0x34c); }
  float x358_unobscureMag() { return *GetField<float>(this, 0x358); }
  bool x3e4_pendingFailsafe() { return *GetField<u8>(this, 0x3e4); }
};
