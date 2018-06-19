#ifndef PRIME_PRACTICE_NATIVE_CTRIGGER_HPP
#define PRIME_PRACTICE_NATIVE_CTRIGGER_HPP

#include "include/MathyTypes.hpp"
#include "PrimeAPI.h"
#include "include/prime/CActor.hpp"

enum ETriggerFlags : u32
{
    None = 0,
    DetectPlayer = (1 << 0),
    DetectAI = (1 << 1),
    DetectProjectiles1 = (1 << 2),
    DetectProjectiles2 = (1 << 3),
    DetectProjectiles3 = (1 << 4),
    DetectProjectiles4 = (1 << 5),
    DetectBombs = (1 << 6),
    DetectPowerBombs = (1 << 7),
    DetectProjectiles5 = (1 << 8),
    DetectProjectiles6 = (1 << 9),
    DetectProjectiles7 = (1 << 10),
    KillOnEnter = (1 << 11),
    DetectMorphedPlayer = (1 << 12),
    UseCollisionImpulses = (1 << 13),
    DetectCamera = (1 << 14),
    UseBooleanIntersection = (1 << 15),
    DetectUnmorphedPlayer = (1 << 16),
    BlockEnvironmentalEffects = (1 << 17)
};

class CScriptTrigger : public CActor {
public:
    ETriggerFlags getFlags() { return *GetField<ETriggerFlags>(this, 0x12c); }
    CAABox *getBounds() { return GetField<CAABox>(this, 0x130); };

};

#endif //PRIME_PRACTICE_NATIVE_CTRIGGER_HPP
