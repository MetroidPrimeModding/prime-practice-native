#pragma once

#include "CSimplePool.hpp"
#include "rstl/list.h"
#include "GetField.hpp"

struct SLoadingData {
  SObjectTag *getTag() { return GetField<SObjectTag>(this, 0x0); };
  u32 getResLen() { return *GetField<u32>(this, 0x2C); };
};

class CResFactory {
public:
  rstl::list<SLoadingData> *getLoadList() { return GetField<rstl::list<SLoadingData>>(this, 0x8C); }
};
