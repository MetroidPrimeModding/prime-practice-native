#pragma once

#include "os.h"
#include "types.h"

template <typename Res> inline Res *GetField(const void *thiz, u32 offset) {
  u32 ptr = ((u32)thiz + offset);
  return (Res *)ptr;
};

inline uint32_t GetVtable(const void *thiz) { return *GetField<uint32_t>(thiz, 0); }

template <typename T> inline T VtableLookup(const void *thiz, int functionOffset) {
  u32 *vtable = *GetField<u32 *>(thiz, 0);
  u32 ptr = vtable[functionOffset + 2];
  return (T)ptr;
}
