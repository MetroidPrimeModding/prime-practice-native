#ifndef PRIME_PRACTICE_CIOWIN_HPP
#define PRIME_PRACTICE_CIOWIN_HPP

#include "rstl/string.h"
#include "prime/CArchitectureMessage.hpp"

class CIOWin {
//    rstl::string x4_name;
//    size_t m_nameHash;
public:
    typedef enum EMessageReturn {
        EMessageReturn_Normal = 0,
        EMessageReturn_Exit = 1,
        EMessageReturn_RemoveIOWinAndExit = 2,
        EMessageReturn_RemoveIOWin = 3
    } EMessageReturn;

//    CIOWin(const rstl::string &name);
//    virtual ~CIOWin();
//    virtual EMessageReturn OnMessage(const CArchitectureMessage &, CArchitectureQueue &);
//    virtual bool GetIsContinueDraw() const;
//    virtual void Draw() const;
//    virtual void PreDraw() const;
//    const rstl::string &GetName() const;
//    size_t GetNameHash() const;
};

#endif //PRIME_PRACTICE_CIOWIN_HPP
