#ifndef PRIME_PRACTICE_CGAMEALLOCATOR_HPP
#define PRIME_PRACTICE_CGAMEALLOCATOR_HPP

#include "types.h"
#include "GetField.hpp"

class CMemoryBlock {
public:
    u32 sentinel;
    u32 size;
    u32 fileAndLinePtr;
    u32 typePtr;
    CMemoryBlock *prev;
    CMemoryBlock *next;
    u32 dataStart;
    u32 canary;
};

#define CGameAllocator_LOCATION ((CGameAllocator*)0x804BFD64)

//class IAllocator {
//public:
//    typedef enum EHint {
//        // Not 100% certain on these
//        DIRECT = 1,
//        MEDIUM = 2,
//        SMALL = 4
//    };
//};

class CGameAllocator {
public:

    u32 heapSize() { return *GetField<u32>(this, 0x8); }

    CMemoryBlock *first() { return *GetField<CMemoryBlock *>(this, 0xC); }

    CMemoryBlock *last() { return *GetField<CMemoryBlock *>(this, 0x10); }

//    void Free(void const *);
//    "Ul"
//    "Q210IAllocator5EHint"
//    "Q210IAllocator6EScope"
//    "Q210IAllocator5EType"
//    "RC10CCallStack"
//    size_t,
//    void *Alloc(unsigned int size, int hint);
};

#endif //PRIME_PRACTICE_CGAMEALLOCATOR_HPP
