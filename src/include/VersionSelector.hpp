#ifndef PRIMEAPI_GETFIELD_HPP
#define PRIMEAPI_GETFIELD_HPP

#include "types.h"

constexpr inline u32 ByVersion(u32 prime1, u32 prime2) {
    static_assert(1 <= PRIME && PRIME <= 2, "Unknown game");

    if constexpr (PRIME == 1) {
        return prime1;
    } else {
        return prime2;
    }
}

#endif