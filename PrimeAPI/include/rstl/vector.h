#ifndef RSTL_VECTOR_H
#define RSTL_VECTOR_H

#include "rstl/rstl.h"

RSTL_BEGIN

template<typename T>
class PACKED vector {
public:
    u32 first; // 0
    u32 len; // 5
    u32 max; // 5
    T *ptr; // ptr
public:

	inline uint32 size() const	{ return len; }
	inline uint32 capacity() const { return max; }
	inline T& operator[](int i) const { return ptr[i]; }
	inline T* data() const { return ptr; }
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
