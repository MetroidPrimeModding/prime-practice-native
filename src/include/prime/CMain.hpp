#pragma once

#include "CGameGlobalObjects.hpp"
#include "GetField.hpp"

typedef enum EFlowState {
  EFlowState_None,
  EFlowState_WinBad,
  EFlowState_WinGood,
  EFlowState_WinBest,
  EFlowState_LoseGame,
  EFlowState_Default,
  EFlowState_StateSetter,
} EFlowState;

class CMain {
public:
  void SetFlowState(EFlowState s) { *(GetField<EFlowState>(this, 0x12c)) = s; };
  // Blocks soft reset, manage card and game exit while set. The memory card driver rewrites it every update
  void SetCardBusy(bool busy) {
    u8 *flags = GetField<u8>(this, 0x160);
    *flags = busy ? (*flags | 1) : (*flags & ~1);
  }
  CGameGlobalObjects *GetGameGlobalObjects() { return *(GetField<CGameGlobalObjects *>(this, 0x128)); }
};

extern CMain *gpMain;
