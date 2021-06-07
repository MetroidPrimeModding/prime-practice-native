#ifndef CGAMEAREA_HPP
#define CGAMEAREA_HPP

#include "prime/CStateManager.hpp"

class CStateManager;

class CGameArea {
public:
    void StartStreamIn(CStateManager&);
    void Validate(CStateManager&);
    void LoadScriptObjects(CStateManager&);

  inline CAssetId mrea() { return *GetField<u32>(this, 0x84); };
  inline u32 areaID() { return *GetField<u32>(this, 0x88); };

};

#endif // CGAMEAREA_HPP
