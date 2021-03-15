#ifndef PRIMEWATCH_PRIME_CFINALINPUT_HPP
#define PRIMEWATCH_PRIME_CFINALINPUT_HPP

#include "GetField.hpp"

class CFinalInput {
public:
    PADDING(0x34);
//    float x0_dt;
//    u32 x4_controllerIdx;
//    float x8_anaLeftX;
//    float xc_anaLeftY;
//    float x10_anaRightX;
//    float x14_anaRightY;
//    float x18_anaLeftTrigger;
//    float x1c_anaRightTrigger;
//
//    /* These were originally per-axis bools, requiring two logical tests
//     * at read-time; now they're logical cardinal-direction states
//     * (negative values indicated) */
//    union {
//        struct {
//            bool x20_enableAnaLeftXP:1;
//            bool x20_enableAnaLeftNegXP:1;
//        };
//        u8 x20;
//    };
//
//    union {
//        struct {
//            bool x21_enableAnaLeftYP:1;
//            bool x21_enableAnaLeftNegYP:1;
//        };
//        u8 x21;
//    };
//
//    union {
//        struct {
//            bool x22_enableAnaRightXP:1;
//            bool x22_enableAnaRightNegXP:1;
//        };
//        u8 x22;
//    };
//
//    union {
//        struct {
//            bool x23_enableAnaRightYP:1;
//            bool x23_enableAnaRightNegYP:1;
//        };
//        u8 x23;
//    };
//
//
//    /* These were originally redundantly-compared floats;
//     * now the logical state is stored directly */
//    float x24_anaLeftTriggerP;
//    float x28_anaRightTriggerP;
////    bool x24_anaLeftTriggerP:1;
////    bool x28_anaRightTriggerP:1;
//
//    union {
//        struct {
//            bool x2c_b24_A:1;
//            bool x2c_b25_B:1;
//            bool x2c_b26_X:1;
//            bool x2c_b27_Y:1;
//            bool x2c_b28_Z:1;
//            bool x2c_b29_L:1;
//            bool x2c_b30_R:1;
//            bool x2c_b31_DPUp:1;
//        };
//        u8 x2c;
//    };
//
//    union {
//        struct {
//            bool x2d_b24_DPRight:1;
//            bool x2d_b25_DPDown:1;
//            bool x2d_b26_DPLeft:1;
//            bool x2d_b27_Start:1;
//            bool x2d_b28_PA:1;
//            bool x2d_b29_PB:1;
//            bool x2d_b30_PX:1;
//            bool x2d_b31_PY:1;
//        };
//        u8 x2d;
//    };
//
//    union {
//        struct {
//            bool x2e_b24_PZ:1;
//            bool x2e_b25_PL:1;
//            bool x2e_b26_PR:1;
//            bool x2e_b27_PDPUp:1;
//            bool x2e_b28_PDPRight:1;
//            bool x2e_b29_PDPDown:1;
//            bool x2e_b30_PDPLeft:1;
//            bool x2e_b31_PStart:1;
//        };
//        u8 x2e;
//    };

    float DeltaTime();
    inline u32 ControllerIdx() { return *GetField<u32>(this, 0x4); };

    bool PStart() const;
    bool PR() const;
    bool PL() const;
    bool PZ() const;
    bool PY() const;
    bool PX() const;
    bool PB() const;
    bool PA() const;
    bool PDPRight() const;
    bool PDPLeft() const;
    bool PDPDown() const;
    bool PDPUp() const;
    bool PRTrigger() const;
    bool PLTrigger() const;
    bool PRARight() const;
    bool PRALeft() const;
    bool PRADown() const;
    bool PRAUp() const;
    bool PLARight() const;
    bool PLALeft() const;
    bool PLADown() const;
    bool PLAUp() const;
    bool DStart() const;
    bool DR() const;
    bool DL() const;
    bool DZ() const;
    bool DY() const;
    bool DX() const;
    bool DB() const;
    bool DA() const;
    bool DDPRight() const;
    bool DDPLeft() const;
    bool DDPDown() const;
    bool DDPUp() const;
    bool DRTrigger() const;
    bool DLTrigger() const;
    bool DRARight() const;
    bool DRALeft() const;
    bool DRADown() const;
    bool DRAUp() const;
    bool DLARight() const;
    bool DLALeft() const;
    bool DLADown() const;
    bool DLAUp() const;
    float AStart() const;
    float AR() const;
    float AL() const;
    float AZ() const;
    float AY() const;
    float AX() const;
    float AB() const;
    float AA() const;
    float ADPRight() const;
    float ADPLeft() const;
    float ADPDown() const;
    float ADPUp() const;
    float ARTrigger() const;
    float ALTrigger() const;
    float ARARight() const;
    float ARALeft() const;
    float ARADown() const;
    float ARAUp() const;
    float ALARight() const;
    float ALALeft() const;
    float ALADown() const;
    float ALAUp() const;

    inline float ALeftX() const { return ALARight() - ALALeft(); };
    inline float ALeftY() const { return ALAUp() - ALADown(); };
    float ARightX() const { return ARARight() - ARALeft(); };
    float ARightY() const { return ARAUp() - ARADown(); };
    float ALeftTrigger() const { return ALTrigger(); };
    float ARightTrigger() const { return ARTrigger(); };
};

#endif //PRIMEWATCH_PRIME_CFINALINPUT_HPP
