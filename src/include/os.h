#pragma once

#include <gccore.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))

extern void OSYieldThread(void);
extern u32 PADRead(PADStatus *status);

extern void OSReport(const char *, ...);
extern s64 OSGetTime();

constexpr s64 TICKS_PER_SECOND = (162000000L / 4L);
constexpr s64 TICKS_PER_MS = TICKS_PER_SECOND / 1000;

#ifdef __cplusplus
}
#endif
