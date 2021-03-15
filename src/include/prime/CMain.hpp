#ifndef PRIME_PRACTICE_CMAIN_HPP
#define PRIME_PRACTICE_CMAIN_HPP

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
};

#endif //PRIME_PRACTICE_CMAIN_HPP
