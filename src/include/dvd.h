#ifndef PRIME_PRACTICE_DVD_H
#define PRIME_PRACTICE_DVD_H

#include "PrimeAPI.h"

#ifdef __cplusplus
extern "C" {
#endif


struct DVDCommandBlock {
    u8 padding[48];
};

struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;      // disk address of file
    u32 length;         // file size in bytes
    u8 padding[4];
};

typedef void (*DVDCallback)(s32 result, DVDFileInfo *fileInfo);

// Funcs (compiled into Prime)

bool DVDOpen(const char *fileName, DVDFileInfo *fileInfo);
bool DVDClose(DVDFileInfo *fileInfo);
bool DVDReadAsyncPrio(DVDFileInfo *file,
                      void *addr,
                      s32 length,
                      s32 offset,
                      DVDCallback callback,
                      s32 priority);

#define DVDGetLength(fileInfo) ((fileInfo)->length)

#ifdef __cplusplus
}
#endif

#endif