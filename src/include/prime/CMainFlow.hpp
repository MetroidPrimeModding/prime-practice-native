#pragma once

#include "CArchitectureQueue.hpp"
#include "prime/CIOWin.hpp"

enum EClientFlowStates {
  kCFS_Unspecified = -1,
  kCFS_PreFrontEnd = 7,
  kCFS_FrontEnd = 8,
  kCFS_Game = 14,
  kCFS_GameExit = 15
};

class CMainFlow {
public:
  CIOWin::EMessageReturn OnMessage(const CArchitectureMessage &msg, CArchitectureQueue &queue);

  int GetGameState() { return *GetField<int>(this, 0x14); }

  void SetGameState(EClientFlowStates state, CArchitectureQueue &queue);
  void AdvanceGameState(CArchitectureQueue &queue);
};
