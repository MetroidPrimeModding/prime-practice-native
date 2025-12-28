#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *prime_calloc(size_t nmemb, size_t size, void *user);
void prime_free(void *ptr, void *user);
void *prime_malloc(size_t size, void *user);
void *prime_realloc(void *ptr, size_t size, void *user);

#ifdef __cplusplus
};
#endif
