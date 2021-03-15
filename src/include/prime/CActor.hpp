#ifndef PRIME_PRACTICE_NATIVE_CACTOR_HPP
#define PRIME_PRACTICE_NATIVE_CACTOR_HPP

#include "PrimeAPI.h"
#include "prime/CEntity.hpp"

class CActor : public CEntity {
public:
    CTransform4f *getTransform() { return GetField<CTransform4f>(this, 0x34); };
};

#endif //PRIME_PRACTICE_NATIVE_CACTOR_HPP
