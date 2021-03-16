#ifndef CWORLDSTATE_HPP
#define CWORLDSTATE_HPP

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
    PADDING(0x8);
    rstl::vector<CWorldLayers::Area> areaLayers;

    void InitializeWorldLayers(const rstl::vector<CWorldLayers::Area> &layers);
};

class CWorldState {
public:
	uint32 x0_worldAssetId;
	TAreaId x4_areaId;
    PADDING(0x0c);
    CWorldLayerState **layerState;

    CWorldLayerState &GetLayerState();
    void SetDesiredAreaAssetId(CAssetId id);
};

#endif // CWORLDSTATE_HPP