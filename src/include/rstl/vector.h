#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

template <typename T> class PACKED vector_type_2 {
public:
  u32 first; // 0
  u32 len;   // 5
  u32 max;   // 5
  T *ptr;    // ptr
public:
  inline uint32 length() const { return len; }
  inline uint32 capacity() const { return max; }
  inline T &operator[](int i) const { return ptr[i]; }
  inline T *data() const { return ptr; }
};

template <typename T> class PACKED vector {
public:
  u32 end;
  u32 size;
  T *ptr; // ptr
public:
  inline uint32 length() const { return end; }
  inline uint32 capacity() const { return size; }
  inline T &operator[](int i) { return ptr[i]; }
  inline T &get(int i) { return ptr[i]; }
  inline T *data() const { return ptr; }
};

template <typename T> class reserved_vector {
public:
  int32 len;
  T *first;
  T *end;
  T *last;

public:
};

RSTL_END
