#ifndef RSTL_VECTOR_H
#define RSTL_VECTOR_H

#include "rstl/rstl.h"

RSTL_BEGIN

template<typename T>
class vector {
public:
    int32 len;
    T *first;
    T *end;
    T *last;
public:

};

template<typename T>
class reserved_vector {
public:
    int32 len;
    T *first;
    T *end;
    T *last;
public:

};

RSTL_END


#endif //PRIME_PRACTICE_STRING_H
