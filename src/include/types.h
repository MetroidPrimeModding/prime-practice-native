#pragma once

#include "PrimeAPI.h"

typedef uint8 u8;
typedef uint16 u16;
// typedef uint32 u32;
typedef int8 s8;
typedef int16 s16;
// typedef int32 s32;
typedef s16 TUniqueId;
typedef uint ResId;
typedef uint CAssetId;

struct TAreaId {
public:
  inline TAreaId() : id(-1) {}
  explicit inline TAreaId(s32 id) : id(id) {}

  s32 id;
};

#define kInvalidAreaId (TAreaId(-1))
