#ifndef RSTL_STRING_H
#define RSTL_STRING_H

#include "rstl/rstl.h"

RSTL_BEGIN

class rmemory_allocator;
template<typename t>
class char_traits;

template<typename _CharTp, typename traits, typename allocator>
class basic_string {
    struct COWData {
        u32 x0_capacity;
        u32 x4_refCount;
        _CharTp *x8_data;
    };

    const _CharTp *x0_ptr;
    COWData *x4_cow;
    u32 x8_size;

    void internal_allocate(int size) {
      x4_cow = reinterpret_cast<COWData *>(new u8[size * sizeof(_CharTp) + 8]);
      x0_ptr = x4_cow->x8_data;
      x4_cow->x0_capacity = u32(size);
      x4_cow->x4_refCount = 1;
    }

    static const _CharTp _EmptyString;

public:
    struct literal_t {
    };

    basic_string(literal_t, const _CharTp *data) {
      x0_ptr = data;
      x4_cow = nullptr;

      const _CharTp *it = data;
      while (*it) {
        ++it;
      }

      x8_size = u32((it - data) / sizeof(_CharTp));
    }

    basic_string(const basic_string &str) {
      x0_ptr = str.x0_ptr;
      x4_cow = str.x4_cow;
      x8_size = str.x8_size;
      if (x4_cow) {
        ++x4_cow->x4_refCount;
      }
    }

    basic_string(const _CharTp *data, int size) {
      if (size <= 0 && !data) {
        x0_ptr = &_EmptyString;
        x4_cow = nullptr;
        x8_size = 0;
        return;
      }

      const _CharTp *it = data;
      u32 len = 0;
      while (*it) {
        if (size != -1 && len >= size) {
          break;
        }
        ++it;
        ++len;
      }

      internal_allocate(len + 1);
      x8_size = len;
      for (int i = 0; i < len; ++i) {
        x4_cow->x8_data[i] = data[i];
      }
      x4_cow->x8_data[len] = 0;
    }

    ~basic_string() {
      if (x4_cow && --x4_cow->x4_refCount == 0) {
        delete[] x4_cow;
      }
    }
};

template<>
const char basic_string<char, char_traits<char>, rmemory_allocator>::_EmptyString = 0;
template<>
const wchar_t basic_string<wchar_t, char_traits<wchar_t>, rmemory_allocator>::_EmptyString = 0;

typedef basic_string<wchar_t, char_traits<wchar_t>, rmemory_allocator> wstring;
typedef basic_string<char, char_traits<char>, rmemory_allocator> string;


inline wstring wstring_l(const wchar_t *data) {
  return wstring(wstring::literal_t(), data);
}

inline string string_l(const char *data) {
  return string(string::literal_t(), data);
}

RSTL_END


#endif //PRIME_PRACTICE_STRING_H
