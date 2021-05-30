#ifndef PRIME_PRACTICE_NATIVE_IMHELPERS_HPP
#define PRIME_PRACTICE_NATIVE_IMHELPERS_HPP

#include "imgui.h"

#define BITFIELD_CHECKBOX(label, bf, ...)                                                                              \
  {                                                                                                                    \
    bool b = (bf);                                                                                                     \
    if (ImGui::Checkbox(label, &b)) {                                                                                  \
      (bf) = b;                                                                                                        \
      __VA_ARGS__                                                                                                      \
    }                                                                                                                  \
  }

#endif //PRIME_PRACTICE_NATIVE_IMHELPERS_HPP
