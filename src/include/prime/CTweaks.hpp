#pragma once

class CTweakGame {
public:
  inline bool *splashScreenDisabled() { return GetField<bool>(this, 0x2b); }
  //  inline CSimplePool *getMainPool() { return GetField<CSimplePool>(this, 0xCC); }
  //  inline CGameState *getGameState() { return *GetField<CGameState*>(this, 0x134); }
};
extern CTweakGame *g_TweakGame;

class CTweakAutoMapper {
public:
  inline float *maxCamDist() { return GetField<float>(this, 0x10); }
  inline float *maxUniverseCamDist() { return GetField<float>(this, 0xd8); }
};
extern CTweakAutoMapper *g_TweakAutoMappers;
