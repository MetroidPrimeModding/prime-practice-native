#ifndef PRIME_PRACTICE_CWORLD_HPP
#define PRIME_PRACTICE_CWORLD_HPP

#include "include/types.h"
#include "include/GetField.hpp"

class CWorld {
public:
    TAreaId GetCurrentAreaId() const { return *(GetField<TAreaId>(this, 0x68)); };
    void SetPauseState(bool paused);
};

#endif //PRIME_PRACTICE_CWORLD_HPP
