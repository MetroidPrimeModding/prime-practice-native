#pragma once

#include "CEntity.hpp"
#include "PrimeAPI.h"

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

  inline CEntity *GetObjectById(TUniqueId uid) {
    if (uid == -1)
      return nullptr;
    SObjectListEntry &ent = entries[uid & 0x3FF];
    // if (!ent.entity || ent.entity->x30_26_scriptingBlocked)
    //   return nullptr;
    return ent.entity;
  }
};
