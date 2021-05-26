
#ifndef DUK_MEM_H
#define DUK_MEM_H

#include <stdlib.h>

//Defines stuff to make duktape work

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

#endif
