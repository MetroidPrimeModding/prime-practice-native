#ifndef PRIME_PRACTICE_CWORLD_HPP
#define PRIME_PRACTICE_CWORLD_HPP

#include "types.h"
#include "rstl/vector.h"
#include "rstl/auto_ptr.h"
#include "GetField.hpp"
#include "prime/CGameArea.hpp"
#include "os.h"

class CMapWorld;

class IWorld {
public:
//  virtual ~IWorld() = default;
//  virtual CAssetId IGetWorldAssetId() const = 0;
//  virtual CAssetId IGetStringTableAssetId() const = 0;
//  virtual CAssetId IGetSaveWorldAssetId() const = 0;
//  virtual const CMapWorld* IGetMapWorld() const = 0;
//  virtual CMapWorld* IGetMapWorld() = 0;
//  virtual IGameArea* IGetAreaAlways(TAreaId id) const = 0;
//  virtual TAreaId IGetCurrentAreaId() const = 0;
//  virtual TAreaId IGetAreaId(CAssetId id) const = 0;
//  virtual bool ICheckWorldComplete() = 0;
//  virtual std::string IGetDefaultAudioTrack() const = 0;
//  virtual int IGetAreaCount() const = 0;



  inline CAssetId IGetWorldAssetId() {
    CAssetId (*fn)(IWorld *) = VtableLookup<CAssetId(*)(IWorld *)>(this, 1);
    return fn(this);
  }

  inline IGameArea *IGetAreaAlways(TAreaId id) {
    return VtableLookup<IGameArea*(*)(IWorld*, TAreaId)>(this, 6)(this, id);
  }

  inline TAreaId IGetCurrentAreaId() {
    TAreaId res;
    // This uses weird CW linkage for struct returns
    void (*fn)(TAreaId &, IWorld *) = VtableLookup<void (*)(TAreaId &, IWorld *)>(this, 7);
    if (fn) {
      fn(res, this);
    }
    return res;
  }
};

class CWorld: public IWorld {
public:
//    inline TAreaId GetAreaId(CAssetId areaAssetId) {
//        TAreaId result(-1);
//        AuxGetAreaId(&result, this, areaAssetId);
//        return result;
//    }

    static void AuxGetAreaId(TAreaId* out, CWorld* obj, CAssetId areaAssetId);

//    CGameArea* IGetAreaAlways(TAreaId id);
//    CAssetId GetWorldId() const { return *(GetField<CAssetId>(this, 0x8)); };
//    TAreaId GetCurrentAreaId() const { return *(GetField<TAreaId>(this, 0x68)); };
    void SetPauseState(bool paused);
    void ScheduleAreaToLoad(CGameArea* area, CStateManager *mgr);
    void MoveToChain(CGameArea* area, EChain chain);
    inline rstl::vector<rstl::auto_ptr<CGameArea>> *areas() { return GetField<rstl::vector<rstl::auto_ptr<CGameArea>>>(this, 0x18); };
};

class CDummyWorld: public IWorld {
public:
//  CDummyGameArea* IGetAreaAlways(TAreaId id);

  inline CAssetId GetWorldId() const { return *(GetField<CAssetId>(this, 0xc)); };
};

#endif //PRIME_PRACTICE_CWORLD_HPP
