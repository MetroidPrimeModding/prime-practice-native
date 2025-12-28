#pragma once

template <typename T> T readValueFromMemory(unsigned int address) { return *reinterpret_cast<const T *>(address); }

template <typename T> void writeValueToMemory(unsigned int address, T value) {
  *reinterpret_cast<T *>(address) = value;
}

// #define SECTION_GAP1 __attribute__((section(".gap1")))
// #pragma clang attribute push (__attribute__((section(".gap1"))), apply_to=function)
// #pragma clang attribute pop

// #if defined(__clang__)
// #pragma clang attribute pop
// #endif

// #if defined(__clang__)
// #pragma clang attribute push (__attribute__((section(".imgui.text"))), apply_to=function)
// #endif
