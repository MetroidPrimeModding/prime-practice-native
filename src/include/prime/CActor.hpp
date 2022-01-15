#pragma once

#include "PrimeAPI.h"
#include "prime/CEntity.hpp"

class CActor : public CEntity {
public:
    CTransform4f *getTransform() { return GetField<CTransform4f>(this, 0x34); };
    u32 getFluidCounter() { return  (*GetField<u32>(this, 0xe4) >> 14) & 0b11; }
    void setFluidCounter(u32 count) {
      u32 *f = GetField<u32>(this, 0xe4);
      u32 mask = ~(0b11 << 14);
      *f = (*f & mask) | ((count & 0b11) << 14);
    }

};
