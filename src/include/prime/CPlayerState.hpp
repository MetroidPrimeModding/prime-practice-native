#pragma once

#include "GetField.hpp"
#include "rstl/vector.h"
#include "types.h"
#include <PrimeAPI.h>
#include <prime/CHealthInfo.hpp>

class CInputStream;

class COutputStream;

class CStateManager;

constexpr int PowerUpMaxValues[] = {
    1, 1, 1, 1,  250, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 14, 1,   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

struct ScanState {
  CAssetId asset;
  float time;
};

class CPlayerState {
public:
  enum class EItemType : s32 {
    Invalid = -1,
    PowerBeam = 0,
    IceBeam = 1,
    WaveBeam = 2,
    PlasmaBeam = 3,
    Missiles = 4,
    ScanVisor = 5,
    MorphBallBombs = 6,
    PowerBombs = 7,
    Flamethrower = 8,
    ThermalVisor = 9,
    ChargeBeam = 10,
    SuperMissile = 11,
    GrappleBeam = 12,
    XRayVisor = 13,
    IceSpreader = 14,
    SpaceJumpBoots = 15,
    MorphBall = 16,
    CombatVisor = 17,
    BoostBall = 18,
    SpiderBall = 19,
    PowerSuit = 20,
    GravitySuit = 21,
    VariaSuit = 22,
    PhazonSuit = 23,
    EnergyTanks = 24,
    UnknownItem1 = 25,
    HealthRefill = 26,
    UnknownItem2 = 27,
    Wavebuster = 28,
    Truth = 29,
    Strength = 30,
    Elder = 31,
    Wild = 32,
    Lifegiver = 33,
    Warrior = 34,
    Chozo = 35,
    Nature = 36,
    Sun = 37,
    World = 38,
    Spirit = 39,
    Newborn = 40,

    /* This must remain at the end of the list */
    Max
  };

  enum class EPlayerVisor : u32 {
    Combat,
    XRay,
    Scan,
    Thermal,

    /* This must remain at the end of the list */
    Max
  };

  enum class EPlayerSuit : s32 {
    Invalid = -1,
    Power,
    Gravity,
    Varia,
    Phazon,
    FusionPower,
    FusionGravity,
    FusionVaria,
    FusionPhazon
  };

  enum class EBeamId : s32 { Invalid = -1, Power, Ice, Wave, Plasma, Phazon, Phazon2 = 27 };

  class CPowerUp {
  public:
    int amount;
    int capacity;

    CPowerUp() {}

    CPowerUp(int, int);
    CPowerUp(CInputStream &);
  };

  /* 0x000 */ int flags;
  /* 0x004 */ int unknown0;
  /* 0x008 */ EBeamId currentBeam;
  /* 0x00C */ CHealthInfo healthInfo;
  /* 0x014 */ EPlayerVisor currentVisor;
  /* 0x018 */ EPlayerVisor transitionVisor;
  /* 0x01C */ float visorTransitionFactor;
  /* 0x020 */ EPlayerSuit currentSuit;
  /* 0x024 */ CPowerUp powerUps[(int)EItemType::Max];
  /* 0x16C */ u32 unknown1;
  /* 0x170 */ rstl::vector_type_2<ScanState> scanStates;

  CPlayerState();
  CPlayerState(CInputStream &);

  inline CHealthInfo *GetHealthInfo() { return &healthInfo; }

  bool CanVisorSeeFog(const CStateManager &) const;
  CPlayerState::EPlayerVisor GetActiveVisor(const CStateManager &) const;
  void UpdateStaticInterference(CStateManager &, const float &);
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

  inline CPowerUp *GetPowerUp(EItemType Item) { return &GetField<CPowerUp>(this, 0x24)[(int)Item]; }

  bool HasPowerUp(CPlayerState::EItemType) const;
  uint GetItemCapacity(CPlayerState::EItemType) const;
  uint GetItemAmount(CPlayerState::EItemType) const;
  void DecrPickUp(CPlayerState::EItemType, int);
  void IncrPickUp(CPlayerState::EItemType, int);
  void AddPowerUp(CPlayerState::EItemType, int);
  void ReInitializePowerUp(CPlayerState::EItemType, int);
  void PutTo(COutputStream &);

  static inline u32 GetPowerUpMaxValue(EItemType type) { return PowerUpMaxValues[size_t(type)]; }

  void ResetAndIncrPickUp(EItemType type, u32 amount);
};

// Max inventory capacity values, indexed by EItemType
extern const uint32 gkPowerUpMaxValues[(int)CPlayerState::EItemType::Max];
