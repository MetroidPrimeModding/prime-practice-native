#pragma once

#include "PrimeAPI.h"

class CRandom16 {
public:
  s32 m_seed;

public:
  explicit CRandom16(s32 seed = 99);

  s32 Next();

  inline s32 GetSeed() const { return m_seed; }

  void SetSeed(s32 seed);

  float Float();

  float Range(float min, float max);

  s32 Range(s32 min, s32 max);
};
