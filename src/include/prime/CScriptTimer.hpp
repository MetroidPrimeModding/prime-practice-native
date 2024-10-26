#pragma once

#include <rstl/vector.h>
#include "MathyTypes.hpp"
#include "PrimeAPI.h"
#include "prime/CEntity.hpp"

class CScriptTimer : public CEntity {
public:
    inline float getTime() { return *GetField<float>(this, 0x34); }
    inline float getStartTime() { return *GetField<float>(this, 0x38); }
    inline float getMaxRandDelay() { return *GetField<float>(this, 0x3C); }
    inline bool getLoop() { return *GetField<bool>(this, 0x40); }
    inline bool getAutoStart() { return *GetField<bool>(this, 0x41); }
    inline bool getIsTiming() { return *GetField<bool>(this, 0x42); }
};
