#ifndef PRIME_PRACTICE_TYPE_H_
#define PRIME_PRACTICE_TYPE_H_

#include "PrimeAPI.h"

typedef uint8 u8;
typedef uint16 u16;
//typedef uint32 u32;
typedef int8 s8;
typedef int16 s16;
//typedef int32 s32;
typedef s16 TUniqueId;
typedef uint ResId;
typedef uint CAssetId;

struct TAreaId {
    s32 id;

    inline TAreaId(s32 v) : id(v) {}
    inline operator s32() const { return id; }
};

#define kInvalidAreaId (TAreaId(-1))

#endif