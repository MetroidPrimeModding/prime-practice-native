#ifndef CSTATEMANAGER_H
#define CSTATEMANAGER_H

#include "PrimeAPI.h"
#include "types.h"
#include "rstl/rc_ptr.h"
#include "rstl/vector.h"
#include "GetField.hpp"
#include "VersionSelector.hpp"
#include "CObjectList.hpp"
#include "CGraphics.hpp"

#define CStateManager_INSTANCE ((CStateManager *) ByVersion(0x8045A1A8, 0x0))

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
    enum class EInitPhase
    {
        LoadWorld = 0,
        LoadFirstArea = 1,
        #if PRIME == 2
        Unknown,
        #endif
        Done
    };
    
    void InitializeState(uint WorldAssetId, TAreaId AreaId, uint AreaAssetId);
    
    inline CPlayer* GetPlayer() const { return *GetField<CPlayer*>(this, ByVersion(0x84C, 0x14FC)); }
    inline EInitPhase GetInitPhase() const { return *GetField<EInitPhase>(this, ByVersion(0xB3C, 0x16f0)); }

    inline CPlayerState *GetPlayerState() const {
        static_assert(1 <= PRIME && PRIME <= 2, "Unknown game");
        if constexpr (PRIME == 1) {
            return GetField<rstl::rc_ptr<CPlayerState>>(this, 0x8b8)->RawPointer();
        } else {
            return *GetField<CPlayerState*>(this, 0x150c);
        }
    }

    inline CWorldLayerState *GetWorldLayerState() const { return worldLayerState.RawPointer(); }

    CWorld *GetWorld() const { return *GetField<CWorld *>(this, ByVersion(0x850, 0x1604)); };

    CWorldTransManager *WorldTransManager() const { return GetField<CWorldTransManager>(this, ByVersion(0x8c4, 0x168C)); }

    CObjectList *GetAllObjs() { return *GetField<CObjectList *>(this, ByVersion(0x810, 2064)); };

    void SetShouldQuitGame(bool should) { GetField<StateManagerFlags>(this, ByVersion(0xf94, 0x0))->xf94_25_quitGame = should; }

    void DrawWorld() const;
    void DrawDebugStuff() const;
    CFrustum SetupViewForDraw(const SViewport& vp) const;
    void ResetViewAfterDraw(const SViewport& backupViewport, const CTransform4f& backupViewMatrix) const;
};

#endif