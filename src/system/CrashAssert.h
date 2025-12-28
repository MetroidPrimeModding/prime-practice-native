#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void CrashAssert(bool v, const char* file, int line);

#ifdef __cplusplus
};
#endif
