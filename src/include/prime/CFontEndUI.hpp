#ifndef PRIME_PRACTICE_NATIVE_CFONTENDUI_HPP
#define PRIME_PRACTICE_NATIVE_CFONTENDUI_HPP

#include <gctypes.h>
#include <GetField.hpp>
#include "prime/CArchitectureQueue.hpp"

class CFrontEndUI {
public:
    u8 filler[256];

    CFrontEndUI(CArchitectureQueue &queue);

    u32 *getPhase() { return GetField<u32>(this, 0x14); }
};

#endif //PRIME_PRACTICE_NATIVE_CFONTENDUI_HPP
