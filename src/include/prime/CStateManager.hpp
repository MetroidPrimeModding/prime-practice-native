#pragma once

#include "CGraphics.hpp"
#include "CObjectList.hpp"
#include "GetField.hpp"
#include "PrimeAPI.h"
#include "rstl/rc_ptr.h"
#include "rstl/vector.h"
#include "types.h"

class CPlayerState;
class CWorldLayerState;
class CGameState;
class CWorldState;
class CWorld;
class CPlayer;
class CCameraManager;
class CWorldTransManager;
class CRandom16;

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
  static inline CStateManager *instance() { return ((CStateManager *)0x8045A1A8); }
  enum EInitPhase { kInit_LoadWorld = 0, kInit_LoadFirstArea = 1, kInit_Done = 2 };

  void InitializeState(uint WorldAssetId, TAreaId AreaId, uint AreaAssetId);
  void Update(float dt);

  inline CPlayer *GetPlayer() const { return *GetField<CPlayer *>(this, 0x84C); }
  inline EInitPhase GetInitPhase() const { return *GetField<EInitPhase>(this, 0xB3C); }
  inline CRandom16 *GetRandom() const { return GetField<CRandom16>(this, 0x8FC); }
  inline CRandom16 *GetActiveRandom() const { return *GetField<CRandom16 *>(this, 0x900); }
  inline CPlayerState *GetPlayerState() const { return mpPlayerState.RawPointer(); }
  inline CWorldLayerState *GetWorldLayerState() const { return worldLayerState.RawPointer(); }
  CWorld *GetWorld() const { return *GetField<CWorld *>(this, 0x850); };
  CWorldTransManager *WorldTransManager() const { return GetField<CWorldTransManager>(this, 0x8c4); }
  CPlayer *Player() const { return *GetField<CPlayer *>(this, 0x84C); };
  CObjectList *GetAllObjs() { return *GetField<CObjectList *>(this, 0x810); };
  CEntity *ObjectById(TUniqueId uid) { return GetAllObjs()->GetObjectById(uid); }
  inline CCameraManager *x870_cameraManager() { return *GetField<CCameraManager *>(this, 0x870); };

  void SetShouldQuitGame(bool should) { GetField<StateManagerFlags>(this, 0xf94)->xf94_25_quitGame = should; }

  void DrawWorld() const;
  void DrawDebugStuff() const;
  CFrustum SetupViewForDraw(const SViewport &vp) const;
  void ResetViewAfterDraw(const SViewport &backupViewport, const CTransform4f &backupViewMatrix) const;
};
