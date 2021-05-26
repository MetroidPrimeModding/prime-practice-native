#include "duk_mem.h"
#include "PrimeAPI.h"
#include "CMemory.h"
#include "types.h"
#include "prime/CGameAllocator.hpp"
#include <string.h>
#include "os.h"

int alloc_bytes = 0;

void *prime_calloc(size_t nmemb, size_t size, void *user) {
  void *res = prime_malloc(nmemb * size, user);
  memset(res, 0, nmemb * size);
  return res;
}

extern "C" {

void prime_free(void *ptr, void *user) {
  delete ptr;
//  CGameAllocator_LOCATION->Free(ptr);
}

void *prime_malloc(size_t size, void *user) {
  OSReport("Alloc %d bytes\n", size);
  char *res = new char[size];
  return res;
//  return CGameAllocator_LOCATION->Alloc(size, 1);
}

}

void *prime_realloc(void *ptr, size_t size, void *user) {
  void *res = prime_malloc(size, user);
  if (res == nullptr) {
    return nullptr;
  }
  if (ptr != nullptr) {
    memcpy(res, ptr, size);
    prime_free(ptr, user);
  }
  return res;
}
