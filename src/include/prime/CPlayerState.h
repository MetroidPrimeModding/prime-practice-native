#ifndef CPLAYERSTATE_H
#define CPLAYERSTATE_H

#include "PrimeAPI.h"
#include "include/prime/CHealthInfo.h"
#include "include/GetField.hpp"

class CPlayerState {
public:
    enum EItemType {
        kItem_PowerBeam = 0,
        kItem_IceBeam = 1,
        kItem_WaveBeam = 2,
        kItem_PlasmaBeam = 3,
        kItem_Missile = 4,
        kItem_ScanVisor = 5,
        kItem_MorphBallBomb = 6,
        kItem_PowerBomb = 7,
        kItem_Flamethrower = 8,
        kItem_ThermalVisor = 9,
        kItem_ChargeBeam = 10,
        kItem_SuperMissile = 11,
        kItem_GrappleBeam = 12,
        kItem_XRayVisor = 13,
        kItem_IceSpreader = 14,
        kItem_SpaceJump = 15,
        kItem_MorphBall = 16,
        kItem_CombatVisor = 17,
        kItem_BoostBall = 18,
        kItem_SpiderBall = 19,
        kItem_PowerSuit = 20,
        kItem_GravitySuit = 21,
        kItem_VariaSuit = 22,
        kItem_PhazonSuit = 23,
        kItem_EnergyTank = 24,
        kItem_Unknown1 = 25,
        kItem_HealthRefill = 26,
        kItem_Unknown2 = 27,
        kItem_Wavebuster = 28,
        kItem_ArtifactOfTruth = 29,
        kItem_ArtifactOfStrength = 30,
        kItem_ArtifactOfElder = 31,
        kItem_ArtifactOfWild = 32,
        kItem_ArtifactOfLifegiver = 33,
        kItem_ArtifactOfWarrior = 34,
        kItem_ArtifactOfChozo = 35,
        kItem_ArtifactOfNature = 36,
        kItem_ArtifactOfSun = 37,
        kItem_ArtifactOfWorld = 38,
        kItem_ArtifactOfSpirit = 39,
        kItem_ArtifactOfNewborn = 40,
        kItem_Max
    };

    enum EPlayerVisor {
        kVisor_Combat = 0,
        kVisor_XRay = 1,
        kVisor_Scan = 2,
        kVisor_Thermal = 3,
        kVisor_Max
    };

    enum EPlayerSuit {
        kSuit_Power = 0,
        kSuit_Gravity = 1,
        kSuit_Varia = 2,
        kSuit_Phazon = 3,
        kSuit_FusionPower = 4,
        kSuit_FusionGravity = 5,
        kSuit_FusionVaria = 6,
        kSuit_FusionPhazon = 7,
        kSuit_Max
    };

    enum EBeamId {
        kBeam_Power = 0,
        kBeam_Ice = 1,
        kBeam_Plasma = 2,
        kBeam_Wave = 3,
        kBeam_Phazon = 4,
        kBeam_Max
    };

    class CPowerUp {
    public:
        int mAmount;
        int mCapacity;
    };

public:
    /* 0x000 */ int mFlags;
    /* 0x004 */ int mUnknown0;
    /* 0x008 */ EBeamId mCurrentBeam;
    /* 0x00C */ CHealthInfo mHealthInfo;
    /* 0x014 */ EPlayerVisor mCurrentVisor;
    /* 0x018 */ EPlayerVisor mTransitionVisor;
    /* 0x01C */ float mVisorTransitionFactor;
    /* 0x020 */ EPlayerSuit mCurrentSuit;
    /* 0x024 */ CPowerUp mPowerUps[kItem_Max];

public:
    inline CHealthInfo *GetHealthInfo() { return &mHealthInfo; }

    inline CPowerUp *GetPowerups() { return GetField<CPowerUp>(this, 0x28); }
    inline CPowerUp *GetPowerUp(EItemType Item) { return &GetField<CPowerUp>(this, 0x24)[Item]; }
};

#endif