#include "PrimeAPI.h"
#include "prime/CMemory.hpp"

void *operator new(size_t size, const char *pkFileAndLine, const char *pkType);
void *operator new[](size_t size, const char *pkFileAndLine, const char *pkType);

void *operator new(size_t size) {
  return operator new(size, "", "");
}

void *operator new[](size_t size) {
  return operator new(size, "", "");
}

void *operator new(size_t size, void* ptr) noexcept {
  return ptr;
}

void *operator new[](size_t size, void* ptr) noexcept {
  return ptr;
}

void operator delete(void *ptr) {
  CMemory::Free(ptr);
}

void operator delete[](void * ptr) {
  ::operator delete(ptr);
}

void operator delete(void * ptr, size_t) {
  ::operator delete(ptr);
}

void operator delete[](void * ptr, size_t) {
  ::operator delete(ptr);
}
