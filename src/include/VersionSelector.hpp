#ifndef PRIMEAPI_GETFIELD_HPP
#define PRIMEAPI_GETFIELD_HPP

#include "types.h"

constexpr inline u32 ByVersion(u32 prime1, u32 prime2) {
#if PRIME == 1
    return prime1;
#elif PRIME == 2
    return prime2;
#else
    #error "Unknown game"
    return 0x0;
#endif
}

#endif