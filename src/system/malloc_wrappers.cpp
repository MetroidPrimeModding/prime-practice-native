#include "malloc_wrappers.h"
#include "PrimeAPI.h"
#include "os.h"
#include <string.h>

void *prime_calloc(size_t nmemb, size_t size, void *user) {
  void *res = prime_malloc(nmemb * size, user);
  memset(res, 0, nmemb * size);
  return res;
}

extern "C" {

static u32 alloc_bytes{0};
static u32 peak{0};

//#define DEBUG_ALLOCS

void prime_free(void *ptr, void *user) {
#ifdef DEBUG_ALLOCS
  if (ptr != nullptr) {
    u32 *root_offset = (u32*)(((char*)ptr) - 32);
    u32 size = *(root_offset);
    alloc_bytes -= size;
    DebugLog("Free: %x %d total %d\n", root_offset, size, alloc_bytes);
    delete root_offset;
  }
#else
  delete ptr;
#endif
}

void *prime_malloc(size_t size, void *user) {
#ifdef DEBUG_ALLOCS
  size += 32;
  char *res = new char[size];
  alloc_bytes += size;
  if (alloc_bytes > peak) {
    peak = alloc_bytes;
  }

  u32 *memory_header = (u32*)(((char*)res));
  *(memory_header) = size;

  DebugLog("Alloc: %x %d total %d peak %d\n", res, size, alloc_bytes, peak);
  return res + 32;
#else
  char *res = new char[size];
  return res;
#endif
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
