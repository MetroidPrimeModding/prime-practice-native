#include "malloc_wrappers.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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

// MSL's formatter behind the game's printf family. It calls write_proc for each chunk of output and returns the
// total length (or -1 if write_proc returns NULL); it never writes a terminator itself.
int __pformatter(void *(*write_proc)(void *ctx, const char *data, size_t len), void *ctx, const char *format,
                 va_list args);

typedef struct {
  char *buf;
  size_t cap; // excluding the terminator
  size_t pos;
} BoundedWriter;

// Unlike the game's __StringWrite, keeps counting past the end so vsnprintf can report the untruncated length.
static void *bounded_write(void *ctx, const char *data, size_t len) {
  BoundedWriter *w = (BoundedWriter *) ctx;
  if (w->pos < w->cap) {
    size_t n = w->cap - w->pos < len ? w->cap - w->pos : len;
    memcpy(w->buf + w->pos, data, n);
  }
  w->pos += len;
  return w;
}

int vsnprintf(char *buf, size_t size, const char *format, va_list args) {
  BoundedWriter w = {buf, size ? size - 1 : 0, 0};
  int len = __pformatter(bounded_write, &w, format, args);
  if (size) {
    buf[w.pos < w.cap ? w.pos : w.cap] = '\0';
  }
  return len;
}

int snprintf(char *buf, size_t size, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buf, size, format, args);
  va_end(args);
  return len;
}
