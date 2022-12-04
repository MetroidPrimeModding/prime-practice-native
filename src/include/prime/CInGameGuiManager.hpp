#pragma once

#include "CAutoMapper.hpp"
#include "../GetField.hpp"

class CInGameGuiManager {
public:
  inline CAutoMapper *getAutoMapper() { return GetField<CAutoMapper>(this, 0x38); };
};
