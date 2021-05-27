#include <stdarg.h>
#include <stddef.h>

int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

int vsnprintf(char* buffer, size_t count, const char* format, va_list va) {
  return vsnprintf_(buffer, count, format, va);
}
