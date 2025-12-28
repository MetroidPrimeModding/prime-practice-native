#pragma once

#include "types.h"

typedef enum ESeekOrigin { ESeekOrigin_Begin = 0, ESeekOrigin_Cur = 1, ESeekOrigin_End = 2 } ESeekOrigin;

class CDvdFile {
  PADDING(0x2000);

public:
  CDvdFile(const char *);
  ~CDvdFile();
  s32 SyncRead(void *dest, unsigned int len);
  s32 CalcFileOffset(int, ESeekOrigin);
};
