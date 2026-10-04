#pragma once

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

constexpr s32 CARD_RESULT_READY = 0;
constexpr s32 CARD_RESULT_BUSY = -1;
constexpr s32 CARD_RESULT_NOCARD = -3;
constexpr s32 CARD_RESULT_NOFILE = -4;
constexpr s32 CARD_RESULT_BROKEN = -6;
constexpr s32 CARD_RESULT_EXIST = -7;

struct CARDFileInfo {
  s32 chan;
  s32 fileNo;
  s32 offset;
  s32 length;
  u16 iBlock;
  u16 padding;
};

struct CARDStat {
  char fileName[32];
  u32 length;
  u32 time;
  u8 gameName[4];
  u8 company[2];
  u8 bannerFormat;
  u8 padding;
  u32 iconAddr;
  u16 iconFormat;
  u16 iconSpeed;
  u32 commentAddr;
  u32 offsetBanner;
  u32 offsetBannerTlut;
  u32 offsetIcon[8];
  u32 offsetIconTlut;
  u32 offsetData;
};
static_assert(sizeof(CARDStat) == 0x6C);

typedef void (*CARDCallback)(s32 chan, s32 result);

s32 CARDGetResultCode(s32 chan);
s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed);
s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize);
s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo);
s32 CARDClose(CARDFileInfo *fileInfo);
s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo, CARDCallback callback);
s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
s32 CARDWriteAsync(CARDFileInfo *fileInfo, const void *buf, s32 length, s32 offset, CARDCallback callback);
s32 CARDDeleteAsync(s32 chan, const char *fileName, CARDCallback callback);
s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat);
s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat *stat, CARDCallback callback);

void DCInvalidateRange(void *addr, u32 nBytes);

#ifdef __cplusplus
}
#endif
