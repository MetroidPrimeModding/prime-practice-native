#ifndef PRIME_PRACTICE_CMAINFLOW_HPP
#define PRIME_PRACTICE_CMAINFLOW_HPP

#include "prime/CIOWin.hpp"

enum class EClientFlowStates {
  Unspecified = -1,
  PreFrontEnd = 7,
  FrontEnd = 8,
  Game = 14,
  GameExit = 15
};

class CMainFlow {
public:
  CIOWin::EMessageReturn OnMessage(const CArchitectureMessage &msg, CArchitectureQueue &queue);

  EClientFlowStates GetGameState() { return *GetField<EClientFlowStates>(this, 0x14); }

  void SetGameState(EClientFlowStates state, CArchitectureQueue &queue);
  void AdvanceGameState(CArchitectureQueue &queue);
};

#endif //PRIME_PRACTICE_CMAINFLOW_HPP
