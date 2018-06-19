#ifndef PRIME_PRACTICE_NATIVE_COBJECTLIST_HPP
#define PRIME_PRACTICE_NATIVE_COBJECTLIST_HPP

#include "PrimeAPI.h"
#include "CEntity.hpp"

class CEntity;

class PACKED SObjectListEntry {
public:
    CEntity *entity;
    u16 next;
    u16 prev;
};

class PACKED CObjectList {
public:
    u32 vtable;
    SObjectListEntry entries[1024];
    u32 type;
    u16 first;
    u16 count;
};

#endif //PRIME_PRACTICE_NATIVE_COBJECTLIST_HPP
