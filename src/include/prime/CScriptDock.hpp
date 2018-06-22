#ifndef PRIME_PRACTICE_NATIVE_CSCRIPTDOCK_HPP
#define PRIME_PRACTICE_NATIVE_CSCRIPTDOCK_HPP

#include "CPhysicsActor.hpp"

class CScriptDock : public CPhysicsActor {
public:
    enum EDockState {
        Idle,
        PlayerTouched,
        EnterNextArea,
        Three
    };

    static constexpr u32 VTABLE_ADDR = 0x803DF580;

    EDockState getState() { return *GetField<EDockState>(this, 0x264); }
};

#endif //PRIME_PRACTICE_NATIVE_CSCRIPTDOCK_HPP
