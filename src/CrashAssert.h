#ifndef PRIME_PRACTICE_NATIVE_CRASHASSERT_H
#define PRIME_PRACTICE_NATIVE_CRASHASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

void CrashAssert(bool v, const char* file, int line);

#ifdef __cplusplus
};
#endif

#endif //PRIME_PRACTICE_NATIVE_CRASHASSERT_H
