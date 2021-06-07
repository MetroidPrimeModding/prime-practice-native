#ifndef PRIME_PRACTICE_CWORLD_HPP
#define PRIME_PRACTICE_CWORLD_HPP

#include "types.h"
#include "rstl/vector.h"
#include "rstl/auto_ptr.h"
#include "GetField.hpp"
#include "prime/CGameArea.hpp"

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
    inline rstl::vector<rstl::auto_ptr<CGameArea>> *areas() { return GetField<rstl::vector<rstl::auto_ptr<CGameArea>>>(this, 0x18); };
};

#endif //PRIME_PRACTICE_CWORLD_HPP
