#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

/** Reference counting pointer. Needs to be fixed so it actually counts references */
template <typename T> class rc_ptr {
  T **mpRawPtr;

public:
  inline T *RawPointer() const {
    if (mpRawPtr == nullptr) {
      return nullptr;
    }
    return *mpRawPtr;
  }
  inline T &operator*() { return **mpRawPtr; }
  inline const T &operator*() const { return **mpRawPtr; }
  inline T &operator->() { return **mpRawPtr; }
  inline const T &operator->() const { return **mpRawPtr; }
  inline operator bool() const { return mpRawPtr != NULL && *mpRawPtr != NULL; }
};

RSTL_END
