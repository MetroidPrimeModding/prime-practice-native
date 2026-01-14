#pragma once

#include <gctypes.h>

struct CARDFileInfo {
  s32 chan;
  s32 fileNo;
  s32 offset;
  s32 length;
  u16 iBlock;
  u16 padding;
};

typedef void (*CARDCallback)(s32 chan, s32 result);

void CARDOpen(s32 chan, char *filename, CARDFileInfo *fileInfo);
s32 CARDReadAsync(CARDFileInfo *fileInfo, void *dest, s32 length, s32 offset, CARDCallback callback);
s32 CARDWriteAsync(CARDFileInfo *fileInfo, const void *src, s32 length, s32 offset, CARDCallback callback);
s32 CARDClose(CARDFileInfo *fileInfo);
s32 CARDMountAsync(s32 chan, void* workArea, CARDCallback detatchCallback, CARDCallback attachCallback);
s32 CARDUnmount(s32 chan);
