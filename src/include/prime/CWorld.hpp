#ifndef PRIME_PRACTICE_CWORLD_HPP
#define PRIME_PRACTICE_CWORLD_HPP

#include "types.h"
#include "GetField.hpp"

class CWorld {
public:
    inline TAreaId GetAreaId(CAssetId areaAssetId) {
        TAreaId result(-1);
        AuxGetAreaId(&result, this, areaAssetId);
        return result;
    }

    static void AuxGetAreaId(TAreaId* out, CWorld* obj, CAssetId areaAssetId);
    
    TAreaId GetCurrentAreaId() const { return *(GetField<TAreaId>(this, 0x68)); };
    void SetPauseState(bool paused);
};

#endif //PRIME_PRACTICE_CWORLD_HPP
