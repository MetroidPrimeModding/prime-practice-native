#pragma once

template <typename T> T readValueFromMemory(unsigned int address) { return *reinterpret_cast<const T *>(address); }

template <typename T> void writeValueToMemory(unsigned int address, T value) {
  *reinterpret_cast<T *>(address) = value;
}
