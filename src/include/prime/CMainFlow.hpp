#ifndef PRIME_PRACTICE_CMAINFLOW_HPP
#define PRIME_PRACTICE_CMAINFLOW_HPP

#include "include/prime/CIOWin.hpp"

class CMainFlow {
public:
    CIOWin::EMessageReturn OnMessage(const CArchitectureMessage& msg, CArchitectureQueue& queue);
};

#endif //PRIME_PRACTICE_CMAINFLOW_HPP
