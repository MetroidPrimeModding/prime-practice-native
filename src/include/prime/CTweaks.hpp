#pragma once


class CTweakGame {
public:
  inline char *splashScreenDisabled() { return GetField<char>(this, 0x2b); }
//  inline CSimplePool *getMainPool() { return GetField<CSimplePool>(this, 0xCC); }
//  inline CGameState *getGameState() { return *GetField<CGameState*>(this, 0x134); }
};
extern CTweakGame *g_TweakGame;
