#ifndef RSTL_RC_PTR_H
#define RSTL_RC_PTR_H

#include "rstl/rstl.h"

RSTL_BEGIN

/** Reference counting pointer. Needs to be fixed so it actually counts references */
template<typename T>
class rc_ptr_prime1
{
    struct rc_data
    {
        T* ptr;
        int refCount;
    };
    rc_data* data;

public:
    inline T* RawPointer() const		{
        if (data) {
            return data->ptr;
        }
        return nullptr;
    }
    inline T& operator*()				{ return *data->ptr; }
    inline const T& operator*() const	{ return *data->ptr; }
    inline T* operator->()				{ return data->ptr; }
    inline const T* operator->() const	{ return data->ptr; }
    inline operator bool() const		{ return data != nullptr && data->ptr != nullptr; }
};

template<typename T>
class rc_ptr_prime2
{
    T* rawPtr;
    int* refCount;

public:
    inline T* RawPointer() const        { return rawPtr; }
    inline T& operator*()				{ return *rawPtr; }
    inline const T& operator*() const	{ return *rawPtr; }
    inline T* operator->()				{ return rawPtr; }
    inline const T* operator->() const	{ return rawPtr; }
    inline operator bool() const		{ return rawPtr != nullptr; }
};


#if PRIME == 1
#define rc_ptr rc_ptr_prime1
#elif PRIME == 2
#define rc_ptr rc_ptr_prime2
#endif

RSTL_END

#endif