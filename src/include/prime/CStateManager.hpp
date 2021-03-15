#ifndef CSTATEMANAGER_H
#define CSTATEMANAGER_H

#include "PrimeAPI.h"
#include "types.h"
#include "rstl/rc_ptr.h"
#include "rstl/vector.h"
#include "GetField.hpp"
#include "prime/CStateManager.hpp"
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
enum EInitPhase
	{
		kInit_LoadWorld = 0,
		kInit_LoadFirstArea = 1,
		kInit_Done = 2
	};
	
	void InitializeState(uint WorldAssetId, TAreaId AreaId, uint AreaAssetId);
	
	inline CPlayer* GetPlayer() const 				{ return *GetField<CPlayer*>(this, 0x84C); }
	inline EInitPhase GetInitPhase() const			{ return *GetField<EInitPhase>(this, 0xB3C); }

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
                                           const CTransform4f& backupViewMatrix) const;
};

#endif