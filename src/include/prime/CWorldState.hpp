#pragma once

#include <PrimeAPI.h>
#include <rstl/vector.h>

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
  rstl::vector<CWorldLayers::Area> areaLayers;

  void InitializeWorldLayers(const rstl::vector<CWorldLayers::Area> &layers);
};

class CWorldState {
public:
  uint32 x0_worldAssetId;
  TAreaId x4_areaId;
  PADDING(0x0c);
  CWorldLayerState **layerState;

  inline CAssetId x10_desiredAreaAssetId() { return *GetField<u32>(this, 0x10); };
  CWorldLayerState &GetLayerState();
  void SetDesiredAreaAssetId(CAssetId id);
};
