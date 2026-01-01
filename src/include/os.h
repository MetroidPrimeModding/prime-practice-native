#pragma once

#include <gccore.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))

extern void OSYieldThread(void);
extern u32 PADRead(PADStatus *status);

extern void OSReport(const char *, ...);
extern s64 OSGetTime();
extern void OSResetSystem(int reset, u32 resetCode, int forceMenu);

constexpr s64 TICKS_PER_SECOND = (162000000L / 4L);
constexpr s64 TICKS_PER_MS = TICKS_PER_SECOND / 1000;

inline void DebugLog(const char* fmt, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    OSReport("DEBUG: ");
    OSReport(fmt, args);
    va_end(args);
#endif
}

#ifdef __cplusplus
}
#endif
