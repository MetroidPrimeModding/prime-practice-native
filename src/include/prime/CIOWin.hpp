#pragma once

#include "prime/CArchitectureMessage.hpp"
#include "rstl/string.h"

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

  inline u32 vtable() { return GetVtable(this); }

  //    CIOWin(const rstl::string &name);
  //    virtual ~CIOWin();
  //    virtual EMessageReturn OnMessage(const CArchitectureMessage &, CArchitectureQueue &);
  //    virtual bool GetIsContinueDraw() const;
  //    virtual void Draw() const;
  //    virtual void PreDraw() const;
  //    const rstl::string &GetName() const;
  //    size_t GetNameHash() const;
};
