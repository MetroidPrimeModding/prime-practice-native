#ifndef CPLAYERSTATE_H
#define CPLAYERSTATE_H

#include <PrimeAPI.h>
#include "rstl/vector.h"
#include <prime/CHealthInfo.hpp>

class CInputStream;
class COutputStream;
class CStateManager;

class CPlayerState {
public:
    enum class EItemType : size_t {
#if PRIME == 1
        PowerBeam = 0,
        IceBeam = 1,
        WaveBeam = 2,
        PlasmaBeam = 3,
        Missile = 4,
        ScanVisor = 5,
        MorphBallBomb = 6,
        PowerBomb = 7,
        Flamethrower = 8,
        ThermalVisor = 9,
        ChargeBeam = 10,
        SuperMissile = 11,
        GrappleBeam = 12,
        XRayVisor = 13,
        IceSpreader = 14,
        SpaceJump = 15,
        MorphBall = 16,
        CombatVisor = 17,
        BoostBall = 18,
        SpiderBall = 19,
        PowerSuit = 20,
        GravitySuit = 21,
        VariaSuit = 22,
        PhazonSuit = 23,
        EnergyTank = 24,
        Unknown1 = 25,
        HealthRefill = 26,
        Unknown2 = 27,
        Wavebuster = 28,
        ArtifactOfTruth = 29,
        ArtifactOfStrength = 30,
        ArtifactOfElder = 31,
        ArtifactOfWild = 32,
        ArtifactOfLifegiver = 33,
        ArtifactOfWarrior = 34,
        ArtifactOfChozo = 35,
        ArtifactOfNature = 36,
        ArtifactOfSun = 37,
        ArtifactOfWorld = 38,
        ArtifactOfSpirit = 39,
        ArtifactOfNewborn = 40,
#elif PRIME == 2
        PowerBeam = 0x0,
        DarkBeam = 0x1,
        LightBeam = 0x2,
        AnnihilatorBeam = 0x3,
        SuperMissile = 0x4,
        Darkburst = 0x5,
        Sunburst = 0x6,
        SonicBoom = 0x7,
        CombatVisor = 0x8,
        ScanVisor = 0x9,
        DarkVisor = 0xa,
        EchoVisor = 0xb,
        VariaSuit = 0xc,
        DarkSuit = 0xd,
        LightSuit = 0xe,
        MorphBall = 0xf,
        BoostBall = 0x10,
        SpiderBall = 0x11,
        MorphBallBombs = 0x12,
        Unknown01 = 0x13,
        Unknown02 = 0x14,
        Unknown03 = 0x15,
        ChargeBeam = 0x16,
        GrappleBeam = 0x17,
        SpaceJumpBoots = 0x18,
        GravityBoost = 0x19,
        SeekerLauncher = 0x1a,
        ScrewAttack = 0x1b,
        Unknown04 = 0x1c,
        TempleKey1 = 0x1d,
        TempleKey2 = 0x1e,
        TempleKey3 = 0x1f,
        AgonKey1 = 0x20,
        AgonKey2 = 0x21,
        AgonKey3 = 0x22,
        TorvusKey1 = 0x23,
        TorvusKey2 = 0x24,
        TorvusKey3 = 0x25,
        HiveKey1 = 0x26,
        HiveKey2 = 0x27,
        HiveKey3 = 0x28,
        HealthRefill = 0x29,
        EnergyTank = 0x2a,
        Powerbomb = 0x2b,
        Missile = 0x2c,
        DarkAmmo = 0x2d,
        LightAmmo = 0x2e,
        ItemPercentage = 0x2f,
        Unknown_48 = 0x30,
        Unknown_49 = 0x31,
        Unknown_50 = 0x32,
        Unknown_51 = 0x33,
        Unknown_52 = 0x34,
        Unknown_53 = 0x35,
        Unknown_54 = 0x36,
        Unknown_55 = 0x37,
        Unknown_56 = 0x38,
        Invisibility = 0x39,
        DoubleDamage = 0x3a,
        Invincibility = 0x3b,
        Unknown_60 = 0x3c,
        Unknown_61 = 0x3d,
        Unknown_62 = 0x3e,
        Unknown_63 = 0x3f,
        Unknown_64 = 0x40,
        Unknown_65 = 0x41,
        Unknown_66 = 0x42,
        Unknown_67 = 0x43,
        Unknown_68 = 0x44,
        Unknown_69 = 0x45,
        Unknown_70 = 0x46,
        Unused1 = 0x47,
        Unused2 = 0x48,
        Unused3 = 0x49,
        Unused4 = 0x4a,
        Unknown_76 = 0x4b,
        Unknown_77 = 0x4c,
        Unknown_78 = 0x4d,
        Unknown_79 = 0x4e,
        Unknown_80 = 0x4f,
        Unknown_81 = 0x50,
        UnlimitedMissiles = 0x51,
        UnlimitedBeamAmmo = 0x52,
        DarkShield = 0x53,
        LightShield = 0x54,
        AbsorbAttack = 0x55,
        DeathBall = 0x56,
        ScanVirus = 0x57,
        Unknown_88 = 0x58,
        DisableBeamAmmo = 0x59,
        DisableMissiles = 0x5a,
        Unknown_91 = 0x5b,
        DisableBall = 0x5c,
        DisableSpaceJump = 0x5d,
        Unknown_94 = 0x5e,
        HackedEffect = 0x5f,
        CannonBall = 0x60,
        VioletTranslator = 0x61,
        AmberTranslator = 0x62,
        EmeraldTranslator = 0x63,
        CobaltTranslator = 0x64,
        TempleKey4 = 0x65,
        TempleKey5 = 0x66,
        TempleKey6 = 0x67,
        TempleKey7 = 0x68,
        TempleKey8 = 0x69,
        TempleKey9 = 0x6a,
        EnergyTransferModule = 0x6b,
        ChargeCombo = 0x6c,
#endif
        Max
    };
    
    enum EPlayerVisor {
        kVisor_Combat = 0,
        kVisor_XRay_Echo = 1,
        kVisor_Scan = 2,
        kVisor_Thermal_Dark = 3,
        kVisor_Max
    };
    
    enum EPlayerSuit {
    #if PRIME == 1
        kSuit_Power = 0,
        kSuit_Gravity = 1,
        kSuit_Varia = 2,
        kSuit_Phazon = 3,
        kSuit_FusionPower = 4,
        kSuit_FusionGravity = 5,
        kSuit_FusionVaria = 6,
        kSuit_FusionPhazon = 7,
    #elif PRIME == 2
        kSuit_Varia = 0,
        kSuit_Dark = 1,
        kSuit_Light = 2,
    #endif
        kSuit_Max
    };
    
    enum EBeamId {
        kBeam_Power = 0,
    #if PRIME == 1
        kBeam_Ice = 1,
        kBeam_Plasma = 2,
        kBeam_Wave = 3,
        kBeam_Phazon = 4,
    #elif PRIME == 2
        kBeam_Dark = 1,
        kBeam_Light = 2,
        kBeam_Annihilator = 3,
    #endif
        kBeam_Max
    };
    
    class CPowerUp {
    public:
        int amount;
        int capacity;
        #if PRIME > 1
        float timeLeft;
        #endif
        
        CPowerUp() {}
        #if PRIME < 2
        CPowerUp(int, int);
        #else
        CPowerUp(int, int, float);
        #endif
        CPowerUp(CInputStream&);
    };
    
    #if PRIME == 2
    int playerIndex;
    #endif
    /* 0x000 */ int flags;
    /* 0x004 */ int unknown0;
    /* 0x008 */ EBeamId currentBeam;
    /* 0x00C */ CHealthInfo healthInfo;
    /* 0x014 */ EPlayerVisor currentVisor;
    /* 0x018 */ EPlayerVisor transitionVisor;
    #if PRIME == 2
    rstl::vector<uint16> vector;
    float relatedToCharge;
    float unk;
    #endif
    /* 0x01C */ float visorTransitionFactor;
    /* 0x020 */ EPlayerSuit currentSuit;
    /* 0x024 */ CPowerUp powerUps[static_cast<size_t>(CPlayerState::EItemType::Max)];
    
    CPlayerState();
    CPlayerState(CInputStream&);
    
    inline CHealthInfo *GetHealthInfo() { return &healthInfo; }
    
    bool CanVisorSeeFog(const CStateManager&) const;
    CPlayerState::EPlayerVisor GetActiveVisor(const CStateManager&) const;
    void UpdateStaticInterference(CStateManager&, const float&);
    void NewScanTime(uint);
    void IncreaseScanTime(uint, float);
    float GetScanTime(uint) const;
    static float GetEnergyTankCapacity();
    static float GetBaseHealthCapacity();
    bool GetIsVisorTransitioning() const;
    float GetVisorTransitionFactor() const;
    void UpdateVisorTransition(float);
    void StartTransitionToVisor(CPlayerState::EPlayerVisor);
    void ResetVisor();
    bool ItemEnabled(CPlayerState::EItemType) const;
    void DisableItem(CPlayerState::EItemType);
    void EnableItem(CPlayerState::EItemType);
    inline CPowerUp *GetPowerUp(EItemType Item) { return &GetField<CPowerUp>(this, 0x24)[static_cast<size_t>(Item)]; }
    bool HasPowerUp(CPlayerState::EItemType) const;
    uint GetItemCapacity(CPlayerState::EItemType) const;
    #if PRIME < 2
    uint GetItemAmount(CPlayerState::EItemType itemType) const;
    #else
    uint GetItemAmount(CPlayerState::EItemType itemType, bool respectFieldToQuery) const;
    #endif
    void DecrPickUp(CPlayerState::EItemType, int);
    void IncrPickUp(CPlayerState::EItemType, int);
    void AddPowerUp(CPlayerState::EItemType, int);
    void ReInitializePowerUp(CPlayerState::EItemType, int);
    void PutTo(COutputStream&);
};

// Max inventory capacity values, indexed by EItemType
extern const uint32 gkPowerUpMaxValues[static_cast<size_t>(CPlayerState::EItemType::Max)];
#if PRIME > 1
extern const char gkPowerUpShouldPersist[static_cast<size_t>(CPlayerState::EItemType::Max)];
#endif

#endif