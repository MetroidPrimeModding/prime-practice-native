#ifndef RSTL_AUTOPTR_H
#define RSTL_AUTOPTR_H

#include "rstl/rstl.h"

RSTL_BEGIN

template<typename T>
class PACKED auto_ptr {
public:
  u32 unknown;
  T *ptr; // ptr
public:

	inline u8 referenced() const	{ return unknown >> 24; }
};

RSTL_END


#endif //RSTL_AUTOPTR_H
