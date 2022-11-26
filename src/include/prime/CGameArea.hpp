#ifndef CGAMEAREA_HPP
#define CGAMEAREA_HPP

#include "prime/CStateManager.hpp"

class CStateManager;

enum class EOcclusionState { Occluded, Visible };
enum class EChain {
  Invalid = -1,
  ToDeallocate,
  Deallocated,
  Loading,
  Alive,
  AliveJudgement,
};

class CPostConstructed {
public:
  float *GetWorldLightingLevel() { return (GetField<float>(this, 0x1128)); };
  EOcclusionState occlusionState() { return *(GetField<EOcclusionState>(this, 0x10dc)); };

};


class CGameArea {
public:
  struct PACKED Dock {
    struct PACKED SDockReference {
      TAreaId x0_area;
      s16 x4_dock;
      u8 x6_loadOther;
    };

      // These are weird, so just going direct
//    s32 GetReferenceCount();
//    TAreaId GetConnectedAreaId(s32 other);
//    bool GetShouldLoadOther(s32 other);
//    void SetShouldLoadOther(s32 other, bool should);
    inline u32 refCount() { return *GetField<u32>(this, 0x8); };
    inline rstl::vector<SDockReference>* refs() { return GetField<rstl::vector<SDockReference>>(this, 0x8); };
    char padding[0x4c];
  };

    void StartStreamIn(CStateManager&);
    void Validate(CStateManager&);
    void LoadScriptObjects(CStateManager&);

  inline CAssetId mrea() { return *GetField<u32>(this, 0x84); };
  inline u32 areaID() { return *GetField<u32>(this, 0x88); };
  inline u32 areaIDx() { return *GetField<u32>(this, 0x4); }; // this is like, the index in the array
  inline rstl::vector<u16>* attachedAreaIndices() { return GetField<rstl::vector<u16>>(this, 0x90); };
  inline rstl::vector<Dock>* docks() { return GetField<rstl::vector<Dock>>(this, 0xD0); };
  inline CPostConstructed *postConstructed() { return *GetField<CPostConstructed*>(this, 0x12C); };
  inline EChain curChain() { return *GetField<EChain>(this, 0x138); };
  void SetOcclusionState(EOcclusionState state);
};

#endif // CGAMEAREA_HPP
