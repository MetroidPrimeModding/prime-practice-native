#pragma once

#include "../GetField.hpp"
#include "CAutoMapper.hpp"

class CInGameGuiManager {
public:
  inline CAutoMapper *getAutoMapper() { return GetField<CAutoMapper>(this, 0x38); };
};
