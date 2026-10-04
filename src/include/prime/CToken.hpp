#pragma once

#include "GetField.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CSimplePool.hpp"
#include "types.h"

class CToken;
// The game's destructor takes a hidden flags argument and frees `this` when it is positive, so it can't be bound to
// a C++ destructor (the caller leaves garbage in that register). Flags must be 0 for an object we own.
extern "C" CToken *CToken_Destroy(CToken *token, s32 flags);

class CToken {
  void *mObjRef;
  int mLockHeld;

public:
  ~CToken() { CToken_Destroy(this, 0); }
  CToken(const CToken &) = delete;
  // Locks the token and builds the object synchronously if needed
  void *GetObj();
  CTexture *texture() {
    // CObjOwnerDerivedFromIObjUntyped: vtable, then the object pointer
    return *GetField<CTexture *>(GetObj(), 0x4);
  }
};
