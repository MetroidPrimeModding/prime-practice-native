#pragma once

class CGameCamera;
class CFirstPersonCamera;
class CBallCamera;

class CCameraManager {
public:
  CFirstPersonCamera *x7c_fpCamera() { return GetField<CFirstPersonCamera>(this, 0x7c); }
  CBallCamera *x80_ballCamera() { return GetField<CBallCamera>(this, 0x80); }
};