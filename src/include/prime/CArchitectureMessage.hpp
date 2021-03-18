#ifndef PRIME_PRACTICE_CARCHITECTUREMESSAGE_HPP
#define PRIME_PRACTICE_CARCHITECTUREMESSAGE_HPP

#include "rstl/rc_ptr.h"
#include "types.h"
#include "prime/CFinalInput.hpp"

class CIOWin;

typedef enum EArchMsgTarget {
    EArchMsgTarget_IOWinManager = 0,
    EArchMsgTarget_Game = 1
} EArchMsgTarget;

typedef enum EArchMsgType {
    EArchMsgType_RemoveIOWin = 0,
    EArchMsgType_CreateIOWin = 1,
    EArchMsgType_ChangeIOWinPriority = 2,
    EArchMsgType_RemoveAllIOWins = 3,
    EArchMsgType_TimerTick = 4,
    EArchMsgType_UserInput = 5,
    EArchMsgType_SetGameState = 6,
    EArchMsgType_ControllerStatus = 7,
    EArchMsgType_QuitGameplay = 8,
    EArchMsgType_FrameBegin = 10,
    EArchMsgType_FrameEnd = 11,
} EArchMsgType;

struct IArchMsgParm {
    u32 vtable;
};

struct CArchMsgParmInt32 : IArchMsgParm {
    u32 x4_parm;
};

struct CArchMsgParmVoidPtr : IArchMsgParm {
    void *x4_parm1;
};

struct CArchMsgParmInt32Int32VoidPtr : IArchMsgParm {
    u32 x4_parm1;
    u32 x8_parm2;
    void *xc_parm3;
};

struct CArchMsgParmInt32Int32IOWin : IArchMsgParm {
    u32 x4_parm1;
    u32 x8_parm2;
    rstl::rc_ptr<CIOWin> xc_parm3;
};

struct CArchMsgParmNull : IArchMsgParm {
};

struct CArchMsgParmReal32 : IArchMsgParm {
    float x4_parm;
};

struct CArchMsgParmUserInput : IArchMsgParm {
    CFinalInput x4_parm;
};

struct CArchMsgParmControllerStatus : IArchMsgParm {
    u16 x4_parm1;
    bool x6_parm2;
};

class CArchitectureMessage {
public:
    EArchMsgTarget x0_target;
    EArchMsgType x4_type;
    rstl::rc_ptr<IArchMsgParm> x8_parm;
};

#endif //PRIME_PRACTICE_CARCHITECTUREMESSAGE_HPP
