#include "stdio.h"
#include "duk_mem.h"

///* Dummy function used in sscanf/swscanf. */
//_READ_WRITE_RETURN_TYPE
//__seofread (struct _reent *_ptr,
//            void *cookie,
//            char *buf,
//            _READ_WRITE_BUFSIZE_TYPE len)
//{
//  return 0;
//}

char *
_findenv_r(struct _reent *reent_ptr,
           register const char *name,
           int *offset) {

  return NULL;
}

/*
 * _getenv_r --
 *	Returns ptr to value associated with name, if any, else NULL.
 */

char *
_getenv_r(struct _reent *reent_ptr,
          const char *name) {
  return NULL;
}

void *malloc(size_t size) {
  return prime_malloc(size, NULL);
}

void free(void *ptr) {
  return prime_free(ptr, NULL);
}

void *_malloc_r(struct _reent *reent_ptr, size_t size) {
  return prime_malloc(size, reent_ptr);
}

void _free_r(struct _reent *reent_ptr, void *size) {
  return prime_free(size, reent_ptr);
}

void *_realloc_r(struct _reent *reent_ptr, void *ptr, size_t size) {
  return prime_realloc(ptr, size, reent_ptr);
}

void *_calloc_r(struct _reent *reent_ptr, size_t nmemb, size_t size) {
  return prime_calloc(nmemb, size, reent_ptr);
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
