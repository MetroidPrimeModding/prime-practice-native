#pragma once

#include "prime/CStateManager.hpp"
#include "prime/CResFactory.hpp"
#include "CSimplePool.hpp"

class CGameGlobalObjects {
public:
  inline CResFactory *getResFactory() { return GetField<CResFactory>(this, 0x4); }
  inline CSimplePool *getMainPool() { return GetField<CSimplePool>(this, 0xCC); }
  inline CGameState *getGameState() { return *GetField<CGameState*>(this, 0x134); }
};
