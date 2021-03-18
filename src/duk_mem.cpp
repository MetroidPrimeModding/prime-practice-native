#include "duk_mem.h"
#include "PrimeAPI.h"
#include "CMemory.h"
#include "types.h"
#include "prime/CGameAllocator.hpp"
#include <string.h>

void *prime_calloc(void *user, size_t nmemb, size_t size) {
  void *res = prime_malloc(user, nmemb * size);
  memset(res, 0, nmemb * size);
  return res;
}

extern "C" {

void prime_free(void *user, void *ptr) {
  delete ptr;
//  CGameAllocator_LOCATION->Free(ptr);
}

void *prime_malloc(void *user, size_t size) {
  char *res = new char[size];
  return res;
//  return CGameAllocator_LOCATION->Alloc(size, 1);
}

}

void *prime_realloc(void *user, void *ptr, size_t size) {
  void *res = prime_malloc(user, size);
  if (res == nullptr) {
    return nullptr;
  }
  if (ptr != nullptr) {
    memcpy(res, ptr, size);
    prime_free(user, ptr);
  }
  return res;
}
