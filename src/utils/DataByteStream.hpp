#pragma once

#include "types.h"

class DataByteStream {
public:
  DataByteStream(void *start, u32 sizeInBytes) : start((u8 *) start), sizeInBytes(sizeInBytes) {}

  u32 getBytes(void *out, u32 len);
  u8 getByte();

  inline bool eof() const { return byteOffset >= sizeInBytes; }
  inline u32 getOffset() const { return byteOffset; }
  inline u32 getSize() const { return sizeInBytes; }
  inline u32 getCrc() const { return crc; }
  inline void resetCrc() { crc = ~0; }

private:
  u8 *start;
  u32 sizeInBytes;
  u32 byteOffset{0};
  u32 crc{0};
};
