#ifndef PRIMEAPI_H
#define PRIMEAPI_H

#include "gccore.h"

// Add this macro at the beginning of _prolog
#define MODULE_INIT \
  extern void ApplyCodePatches(); \
  ApplyCodePatches();

// Patches all references to originalSymbol in the base game to point to newSymbol instead
#define PATCH_SYMBOL(originalSymbol, newSymbol)

// Common typedefs
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef signed long long int64;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef unsigned long ulong;
//typedef unsigned long size_t;
typedef float f32;
typedef double f64;

// Common defines
#ifndef NULL
#define NULL 0
#endif
#ifndef nullptr
#define nullptr NULL
#endif

#define PADDING(Amt) char padding##Amt[ ( Amt ) ]

#define VALID_PTR(p) ((u32)p >= 0x80000000 && (u32)p <= 0x81800000)

#define PACKED __attribute__ ((__packed__))

// Allocation
#ifdef __cplusplus
void *operator new(size_t size);
void *operator new(size_t size, void* ptr);

void operator delete(void *);

#if __cplusplus >= 201402L

inline void operator delete(void * ptr, size_t) {
	::operator delete(ptr);
}


inline void operator delete[](void * ptr, size_t) {
	::operator delete(ptr);
}

#endif

//#define new new("", "")

#endif

#endif
