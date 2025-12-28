#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

template <typename T> class PACKED list_node {
public:
  list_node<T> *prev;
  list_node<T> *next;
  T item;
};

template <typename T> class PACKED list {
public:
  list_node<T> *end;
  list_node<T> *last;
  list_node<T> *first;
  u32 size;
};

RSTL_END
