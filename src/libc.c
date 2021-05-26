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
_findenv_r (struct _reent *reent_ptr,
            register const char *name,
            int *offset)
{

  return NULL;
}

/*
 * _getenv_r --
 *	Returns ptr to value associated with name, if any, else NULL.
 */

char *
_getenv_r (struct _reent *reent_ptr,
           const char *name)
{
  return NULL;
}

void	*malloc(size_t size) {
  return prime_malloc(size, NULL);
}

void free(void *ptr) {
  return prime_free(ptr, NULL);
}

void *_malloc_r (struct _reent *reent_ptr, size_t size) {
  return prime_malloc(size, reent_ptr);
}

void _free_r (struct _reent *reent_ptr, void *size) {
  return prime_free(size, reent_ptr);
}

void *_realloc_r (struct _reent *reent_ptr, void *ptr, size_t size) {
  return prime_realloc(ptr, size, reent_ptr);
}

void *_calloc_r (struct _reent *reent_ptr, size_t nmemb, size_t size) {
  return prime_calloc(nmemb, size, reent_ptr);
}

// basic impl of pure virtual error so we can use virtual methods
void __cxa_pure_virtual() { while (1); }

unsigned long __umoddi3(unsigned long a, unsigned long b) {
  return a % b;
}

unsigned long __udivdi3 (unsigned long a, unsigned long b) {
  return a / b;
}

int	_putchar(int c) {
  // don't put it
  return c;
}
