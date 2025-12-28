#pragma once

#include "PrimeAPI.h"
#include "prime/CEntity.hpp"

class CScannableObjectInfo {
public:
  CAssetId x0_scannableObjectId;
  CAssetId x4_stringId;
  float x8_totalDownloadTime = 0.f;
  u32 xc_category = 0;
  bool x10_important = false;
};

class CActor : public CEntity {
public:
    CTransform4f *getTransform() { return GetField<CTransform4f>(this, 0x34); };
    u32 getFluidCounter() { return  (*GetField<u32>(this, 0xe4) >> 14) & 0b11; }
    void setFluidCounter(u32 count) {
      u32 *f = GetField<u32>(this, 0xe4);
      u32 mask = ~(0b11 << 14);
      *f = (*f & mask) | ((count & 0b11) << 14);
    }

  const CScannableObjectInfo* GetScannableObjectInfo() const;

};
