#ifndef PRIME_PRACTICE_GETFIELD_HPP
#define PRIME_PRACTICE_GETFIELD_HPP

#include "types.h"

template<typename Res>
inline Res *GetField(const void *thiz, u32 offset) {
  u32 ptr = ((u32)thiz + offset);
  return (Res*)ptr;
};

inline uint32_t GetVtable(const void *thiz) {
  return *GetField<uint32_t>(thiz, 0);
}

#endif //PRIME_PRACTICE_GETFIELD_HPP
