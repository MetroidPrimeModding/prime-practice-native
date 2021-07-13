#ifndef PRIME_PRACTICE_NATIVE_CCAMERABOBBER_HPP
#define PRIME_PRACTICE_NATIVE_CCAMERABOBBER_HPP

#include "GetField.hpp"
#include "MathyTypes.hpp"

class CCameraBobber {
public:
  float *getTargetBobMagnitude() { return GetField<float>(this, 0x10); }
  float *getBobMagnitude() { return GetField<float>(this, 0x14); }
  float *getBobTimescale() { return GetField<float>(this, 0x18); }
  float *getBobTime() { return GetField<float>(this, 0x1C); }
  CTransform4f *getCameraBobTransform() { return GetField<CTransform4f>(this, 0x38); }
};

#endif //PRIME_PRACTICE_NATIVE_CCAMERABOBBER_HPP
