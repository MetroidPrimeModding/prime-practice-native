#ifndef PRIME_PRACTICE_CWORLD_HPP
#define PRIME_PRACTICE_CWORLD_HPP

#include "types.h"
#include "GetField.hpp"

class CWorld {
public:
    TAreaId IGetAreaId(TAreaId areaAssetId) const;
    
    TAreaId GetCurrentAreaId() const { return *(GetField<TAreaId>(this, 0x68)); };
    void SetPauseState(bool paused);
};

#endif //PRIME_PRACTICE_CWORLD_HPP
