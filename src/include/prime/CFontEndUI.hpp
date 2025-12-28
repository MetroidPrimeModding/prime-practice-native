#pragma once

#include "prime/CArchitectureQueue.hpp"
#include <GetField.hpp>
#include <gctypes.h>

class CFrontEndUI {
public:
  u8 filler[256];

  CFrontEndUI(CArchitectureQueue &queue);

  u32 *getPhase() { return GetField<u32>(this, 0x14); }
};
