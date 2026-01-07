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
  CGameGlobalObjects *GetGameGlobalObjects() { return *(GetField<CGameGlobalObjects *>(this, 0x128)); }
};

extern CMain *gpMain;
