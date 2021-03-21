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
    enum EItemType {
#if PRIME == 1
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
#elif PRIME == 2
        kItem_PowerBeam = 0x0,
        kItem_DarkBeam = 0x1,
        kItem_LightBeam = 0x2,
        kItem_AnnihilatorBeam = 0x3,
        kItem_SuperMissile = 0x4,
        kItem_Darkburst = 0x5,
        kItem_Sunburst = 0x6,
        kItem_SonicBoom = 0x7,
        kItem_CombatVisor = 0x8,
        kItem_ScanVisor = 0x9,
        kItem_DarkVisor = 0xa,
        kItem_EchoVisor = 0xb,
        kItem_VariaSuit = 0xc,
        kItem_DarkSuit = 0xd,
        kItem_LightSuit = 0xe,
        kItem_MorphBall = 0xf,
        kItem_BoostBall = 0x10,
        kItem_SpiderBall = 0x11,
        kItem_MorphBallBombs = 0x12,
        kItem_Unknown01 = 0x13,
        kItem_Unknown02 = 0x14,
        kItem_Unknown03 = 0x15,
        kItem_ChargeBeam = 0x16,
        kItem_GrappleBeam = 0x17,
        kItem_SpaceJumpBoots = 0x18,
        kItem_GravityBoost = 0x19,
        kItem_SeekerLauncher = 0x1a,
        kItem_ScrewAttack = 0x1b,
        kItem_Unknown04 = 0x1c,
        kItem_TempleKey1 = 0x1d,
        kItem_TempleKey2 = 0x1e,
        kItem_TempleKey3 = 0x1f,
        kItem_AgonKey1 = 0x20,
        kItem_AgonKey2 = 0x21,
        kItem_AgonKey3 = 0x22,
        kItem_TorvusKey1 = 0x23,
        kItem_TorvusKey2 = 0x24,
        kItem_TorvusKey3 = 0x25,
        kItem_HiveKey1 = 0x26,
        kItem_HiveKey2 = 0x27,
        kItem_HiveKey3 = 0x28,
        kItem_HealthRefill = 0x29,
        kItem_EnergyTank = 0x2a,
        kItem_Powerbomb = 0x2b,
        kItem_Missile = 0x2c,
        kItem_DarkAmmo = 0x2d,
        kItem_LightAmmo = 0x2e,
        kItem_ItemPercentage = 0x2f,
        kItem_Unknown_48 = 0x30,
        kItem_Unknown_49 = 0x31,
        kItem_Unknown_50 = 0x32,
        kItem_Unknown_51 = 0x33,
        kItem_Unknown_52 = 0x34,
        kItem_Unknown_53 = 0x35,
        kItem_Unknown_54 = 0x36,
        kItem_Unknown_55 = 0x37,
        kItem_Unknown_56 = 0x38,
        kItem_Invisibility = 0x39,
        kItem_DoubleDamage = 0x3a,
        kItem_Invincibility = 0x3b,
        kItem_Unknown_60 = 0x3c,
        kItem_Unknown_61 = 0x3d,
        kItem_Unknown_62 = 0x3e,
        kItem_Unknown_63 = 0x3f,
        kItem_Unknown_64 = 0x40,
        kItem_Unknown_65 = 0x41,
        kItem_Unknown_66 = 0x42,
        kItem_Unknown_67 = 0x43,
        kItem_Unknown_68 = 0x44,
        kItem_Unknown_69 = 0x45,
        kItem_Unknown_70 = 0x46,
        kItem_Unused1 = 0x47,
        kItem_Unused2 = 0x48,
        kItem_Unused3 = 0x49,
        kItem_Unused4 = 0x4a,
        kItem_Unknown_76 = 0x4b,
        kItem_Unknown_77 = 0x4c,
        kItem_Unknown_78 = 0x4d,
        kItem_Unknown_79 = 0x4e,
        kItem_Unknown_80 = 0x4f,
        kItem_Unknown_81 = 0x50,
        kItem_UnlimitedMissiles = 0x51,
        kItem_UnlimitedBeamAmmo = 0x52,
        kItem_DarkShield = 0x53,
        kItem_LightShield = 0x54,
        kItem_AbsorbAttack = 0x55,
        kItem_DeathBall = 0x56,
        kItem_ScanVirus = 0x57,
        kItem_Unknown_88 = 0x58,
        kItem_DisableBeamAmmo = 0x59,
        kItem_DisableMissiles = 0x5a,
        kItem_Unknown_91 = 0x5b,
        kItem_DisableBall = 0x5c,
        kItem_DisableSpaceJump = 0x5d,
        kItem_Unknown_94 = 0x5e,
        kItem_HackedEffect = 0x5f,
        kItem_CannonBall = 0x60,
        kItem_VioletTranslator = 0x61,
        kItem_AmberTranslator = 0x62,
        kItem_EmeraldTranslator = 0x63,
        kItem_CobaltTranslator = 0x64,
        kItem_TempleKey4 = 0x65,
        kItem_TempleKey5 = 0x66,
        kItem_TempleKey6 = 0x67,
        kItem_TempleKey7 = 0x68,
        kItem_TempleKey8 = 0x69,
        kItem_TempleKey9 = 0x6a,
        kItem_EnergyTransferModule = 0x6b,
        kItem_ChargeCombo = 0x6c,
#endif
        kItem_Max
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
    /* 0x024 */ CPowerUp powerUps[kItem_Max];
    
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
    inline CPowerUp *GetPowerUp(EItemType Item) { return &GetField<CPowerUp>(this, 0x24)[Item]; }
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
extern const uint32 gkPowerUpMaxValues[CPlayerState::EItemType::kItem_Max];
#if PRIME > 1
extern const char gkPowerUpShouldPersist[CPlayerState::EItemType::kItem_Max];
#endif

#endif