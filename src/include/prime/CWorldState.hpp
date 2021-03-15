#ifndef CWORLDSTATE_HPP
#define CWORLDSTATE_HPP

#include <PrimeAPI.h>
#include <rstl/vector.h>

class CWorldLayers
{
public:
	struct Area
	{
		uint32 mStartNameIdx;
		uint32 mLayerCount;
		uint64 mLayerBits;
	};
};

class CWorldLayerState
{
public:
    rstl::vector<CWorldLayers::Area> areaLayers;
	
    void InitializeWorldLayers(const rstl::vector<CWorldLayers::Area> &layers);
};

class CWorldState
{
public:
	uint32 x0_worldAssetId;
	TAreaId x4_areaId;
	
	inline CWorldLayerState* GetWorldLayerState() const	{ return **GetField<CWorldLayerState**>(this, 0x14); }
	void SetDesiredAreaAssetId(uint assetId);
};

#endif // CWORLDSTATE_HPP