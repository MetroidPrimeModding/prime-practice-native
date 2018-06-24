#ifndef PRIME_PRACTICE_NATIVE_CSCRIPTDOOR_HPP
#define PRIME_PRACTICE_NATIVE_CSCRIPTDOOR_HPP

#include "CPhysicsActor.hpp"

class CScriptDoor : public CPhysicsActor {
public:
    static constexpr u32 VTABLE_ADDR = 0x803daa78;
};

#endif //PRIME_PRACTICE_NATIVE_CSCRIPTDOOR_HPP
