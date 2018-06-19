#ifndef PRIME_PRACTICE_NATIVE_CACTOR_HPP
#define PRIME_PRACTICE_NATIVE_CACTOR_HPP

#include "PrimeAPI.h"
#include "include/prime/CEntity.hpp"

class CActor : public CEntity {
public:
    CTransform *getTransform() { return GetField<CTransform>(this, 0x34); };
};

#endif //PRIME_PRACTICE_NATIVE_CACTOR_HPP
