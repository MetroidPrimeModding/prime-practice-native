#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

template <typename A, typename B> class PACKED pair {
public:
  A a;
  B b;
};

RSTL_END
