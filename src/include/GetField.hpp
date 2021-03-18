#ifndef PRIME_PRACTICE_GETFIELD_HPP
#define PRIME_PRACTICE_GETFIELD_HPP

#include "types.h"

template<typename Res>
Res *GetField(const void *thiz, u32 offset) {
  u32 ptr = ((u32)thiz + offset);
  return (Res*)ptr;
};

#endif //PRIME_PRACTICE_GETFIELD_HPP
