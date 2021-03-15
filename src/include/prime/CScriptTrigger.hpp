#ifndef PRIME_PRACTICE_NATIVE_CTRIGGER_HPP
#define PRIME_PRACTICE_NATIVE_CTRIGGER_HPP

#include <rstl/vector.h>
#include "MathyTypes.hpp"
#include "PrimeAPI.h"
#include "prime/CActor.hpp"

enum ETriggerFlags : u32 {
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

enum PACKED ETriggerType : u8 {
    NotYetDetermined = 0,
    Unknown,
    Load,
    Door,
    Force,
    CameraHint
};

class CScriptTrigger : public CActor {
public:
    static constexpr u32 VTABLE_ADDR = 0x803da4d8;

    inline ETriggerFlags getFlags() { return *GetField<ETriggerFlags>(this, 0x12c); }

    inline CAABox *getBounds() { return GetField<CAABox>(this, 0x130); };

    inline u8 *getTriggerType() { return GetField<u8>(this, 0x149); }

    inline float getForceMagnitude() { return *GetField<float>(this, 0x128); }

    inline CVector3f *getForceField() { return GetField<CVector3f>(this, 0x11c); }
};

#endif //PRIME_PRACTICE_NATIVE_CTRIGGER_HPP
