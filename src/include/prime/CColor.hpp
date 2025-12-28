#pragma once

#include "types.h"
#include <PrimeAPI.h>

typedef enum EColorType {
  EColorType_Main,
  EColorType_Outline,
  EColorType_Geometry,
  EColorType_Foreground,
  EColorType_Background
} EColorType;
class CColor {
public:
  static CColor Orange();
  static CColor Yellow();
  static CColor Blue();
  static CColor Red();
  static CColor Grey();
  static CColor White();
  static CColor Black();
  CColor(float r, float g, float b, float a);
  u32 color;
};
