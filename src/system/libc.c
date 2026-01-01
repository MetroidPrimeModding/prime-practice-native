#include "malloc_wrappers.h"

// char *
//_findenv_r(struct _reent *reent_ptr,
//           register const char *name,
//           int *offset) {
//
//  return NULL;
//}
//
///*
// * _getenv_r --
// *	Returns ptr to value associated with name, if any, else NULL.
// */
//
//char *
//_getenv_r(struct _reent *reent_ptr,
//          const char *name) {
//  return NULL;
//}

void *malloc(size_t size) {
  return prime_malloc(size, NULL);
}

void free(void *ptr) {
  return prime_free(ptr, NULL);
}

// basic impl of pure virtual error so we can use virtual methods
void __cxa_pure_virtual() { while (1); }

int _putchar(int c) {
  // don't put it
  return c;
}

char *
strstr(const char *hs, const char *ne) {
  size_t i;
  int c = ne[0];

  if (c == 0)
    return (char *) hs;

  for (; hs[0] != '\0'; hs++) {
    if (hs[0] != c)
      continue;
    for (i = 1; ne[i] != 0; i++)
      if (hs[i] != ne[i])
        break;
    if (ne[i] == '\0')
      return (char *) hs;
  }

  return NULL;
}

double atan2(double a, double b);

float atan2f(float a, float b) {
  return atan2(a, b);
}

int toupper(int c) {
  return c & 0b11011111;
}
