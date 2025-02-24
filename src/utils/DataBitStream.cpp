#include <string.h>
#include "DataByteStream.hpp"

u32 crc32(const u8* data, u32 length, u32 previousCrc32 = 0) {
  u32 crc = ~previousCrc32;
  while (length--) {
    crc ^= *data++;
    for (int k = 0; k < 8; k++) {
      crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
    }
  }
  return ~crc;
}

u32 DataByteStream::getBytes(void* out, u32 len) {
  if (byteOffset + len > sizeInBytes) {
    len = sizeInBytes - byteOffset;
  }
  memcpy(out, start + byteOffset, len);
  crc = crc32((const uint8_t*)out, len, crc);
  byteOffset += len;
  return len;
}
