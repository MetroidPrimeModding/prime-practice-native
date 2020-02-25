#ifndef PRIME_PRACTICE_OS_H
#define PRIME_PRACTICE_OS_H

#include <gccore.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void OSYieldThread(void);
extern u32 PADRead(PADStatus *status);

extern void OSReport(const char*);


#ifdef __cplusplus
}
#endif

#endif