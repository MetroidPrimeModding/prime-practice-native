#ifndef CSTATEMANAGER_H
#define CSTATEMANAGER_H

#include "PrimeAPI.h"
#include "include/types.h"
#include "rstl/rc_ptr.h"
#include "rstl/vector.h"
#include "include/GetField.hpp"
#include "include/prime/CStateManager.h"
#include "CObjectList.hpp"
#include "CGraphics.hpp"

#define CStateManager_INSTANCE ((CStateManager *) 0x8045A1A8)

class CPlayerState;

class CWorldLayerState;

class CGameState;

class CWorldState;

class CWorld;

class CPlayer;

class CWorldTransManager;

struct StateManagerFlags {
    bool xf94_24_ : 1;
    bool xf94_25_quitGame : 1;
    bool xf94_26_generatingObject : 1;
    bool xf94_27_inMapScreen : 1;
    bool xf94_28_inSaveUI : 1;
    bool xf94_29_cinematicPause : 1;
    bool xf94_30_fullThreat : 1;
};

class CStateManager {
    PADDING(0x8b8);
    rstl::rc_ptr<CPlayerState> mpPlayerState;
    PADDING(12);
    rstl::rc_ptr<CWorldLayerState> worldLayerState;

public:
    inline CPlayerState *GetPlayerState() const { return mpPlayerState.RawPointer(); }

    inline CWorldLayerState *GetWorldLayerState() const { return worldLayerState.RawPointer(); }

    CWorld *GetWorld() const { return *GetField<CWorld *>(this, 0x850); };

    CWorldTransManager *WorldTransManager() const { return GetField<CWorldTransManager>(this, 0x8c4); }

    CPlayer *Player() const { return *GetField<CPlayer *>(this, 0x84C); };

    CObjectList *GetAllObjs() { return *GetField<CObjectList *>(this, 0x810); };

    void SetShouldQuitGame(bool should) { GetField<StateManagerFlags>(this, 0xf94)->xf94_25_quitGame = should; }

    void DrawWorld() const;
    void DrawDebugStuff() const;
    CFrustum SetupViewForDraw(const SViewport& vp) const;
    void ResetViewAfterDraw(const SViewport& backupViewport,
                                           const CTransform& backupViewMatrix) const;
};

class CGameState {
public:
    CWorldState &StateForWorld(unsigned int world);
    void SetCurrentWorldId(CAssetId id);
    CWorldState &CurrentWorldState();


    inline CAssetId MLVL() { return *GetField<u32>(this, 0x84); };

    inline double PlayTime() { return *GetField<double>(this, 0xa0); };
};

class CWorldState {
public:
    PADDING(0x14);
    CWorldLayerState **layerState;

    CWorldLayerState &GetLayerState();
    void SetDesiredAreaAssetId(CAssetId id);
};

class CWorldLayers {
public:
    struct Area {
        uint32 m_startNameIdx;
        uint32 m_layerCount;
        uint64 m_layerBits;
    };
};

class CWorldLayerState {
public:
    PADDING(0x8);
    rstl::vector<CWorldLayers::Area> areaLayers;

    void InitializeWorldLayers(const rstl::vector<CWorldLayers::Area> &layers);
};

#endif