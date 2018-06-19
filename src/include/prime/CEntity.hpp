#ifndef PRIME_PRACTICE_NATIVE_CENTITY_HPP
#define PRIME_PRACTICE_NATIVE_CENTITY_HPP

#include "PrimeAPI.h"
#include "include/GetField.hpp"

class CEntity {
public:
    static constexpr uint8_t ACTIVE_MASK = 0x80;
    static constexpr uint8_t GRAVEYARD_MASK = 0x40;
    static constexpr uint8_t BLOCKED_MASK = 0x20;
    static constexpr uint8_t USE_MASK = 0x10;

    inline u32 getVtablePtr() { return *GetField<u32>(this, 0x0); }
    inline u32 getAreaID() { return *GetField<u32>(this, 0x4); }
    inline u16 getUniqueID() { return *GetField<u16>(this, 0x8); }
    inline u32 getEditorID() { return *GetField<u32>(this, 0xC); }
    inline u8 getStatus() { return *GetField<u8>(this, 0x30); }

};

#endif //PRIME_PRACTICE_NATIVE_CENTITY_HPP
