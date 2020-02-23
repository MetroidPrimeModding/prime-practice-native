#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <new>

extern "C" {

extern void free(const void *ptr);

const uint8_t *hello();

extern void *malloc(uint32_t len);

extern void printf(const uint8_t *fmt);

extern void sprintf(uint8_t *s, const uint8_t *fmt);

} // extern "C"
