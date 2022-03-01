#ifndef CGAMEAREA_HPP
#define CGAMEAREA_HPP

#include "prime/CStateManager.hpp"

class CStateManager;

class CPostConstructed {
public:
  float *GetWorldLightingLevel() { return (GetField<float>(this, 0x1128)); };
};


class CGameArea {
public:
    void StartStreamIn(CStateManager&);
    void Validate(CStateManager&);
    void LoadScriptObjects(CStateManager&);

  inline CAssetId mrea() { return *GetField<u32>(this, 0x84); };
  inline u32 areaID() { return *GetField<u32>(this, 0x88); };
  inline CPostConstructed *postConstructed() { return *GetField<CPostConstructed*>(this, 0x12C); };
};

#endif // CGAMEAREA_HPP
