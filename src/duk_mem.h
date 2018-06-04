
#ifndef DUK_MEM_H
#define DUK_MEM_H

#include <stdlib.h>

//Defines stuff to make duktape work

#ifdef __cplusplus
extern "C" {
#endif

void *prime_calloc(void *user, size_t nmemb, size_t size);
void prime_free(void *user, void *ptr);
void *prime_malloc(void *user, size_t size);
void *prime_realloc(void *user, void * ptr, size_t size);

#ifdef __cplusplus
};
#endif

#endif