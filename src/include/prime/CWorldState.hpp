#ifndef CWORLDSTATE_HPP
#define CWORLDSTATE_HPP

#include <PrimeAPI.h>
#include <rstl/vector.h>
#include <rstl/rc_ptr.h>

class CRelayTracker;
class CMapWorldInfo;

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
    rstl::rc_ptr<CRelayTracker> relayTracker;
    rstl::rc_ptr<CMapWorldInfo> mapWorldInfo;
    CAssetId desiredAreaAssetId;
    rstl::rc_ptr<CWorldLayerState> layerState;

    rstl::rc_ptr<CWorldLayerState>& GetLayerState();
    void SetDesiredAreaAssetId(CAssetId id);
};

#endif // CWORLDSTATE_HPP