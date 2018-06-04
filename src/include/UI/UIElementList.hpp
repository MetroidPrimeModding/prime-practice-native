#ifndef PRIME_PRACTICE_UIELEMENTLIST_HPP
#define PRIME_PRACTICE_UIELEMENTLIST_HPP

#include "include/types.h"

class UIElement;

/**
 * Takes ownership of the pointers it holds & will deconstruct them!
 */
class UIElementList {
private:
    UIElement **arr;
    u32 size;
public:
    inline UIElementList() {
      size = 0;
      arr = new UIElement *[size];
    }

    inline ~UIElementList() {
      resize(0);
      delete(arr);
      arr = nullptr;
    }

    inline void add(UIElement *ele) {
      for (u32 i = 0; i < size; i++) {
        if (arr[i] == nullptr) {
          arr[i] = ele;
          return;
        }
      }
      resize(size + 1);
      arr[size - 1] = ele;
    }

    inline UIElement *&operator[](u32 idx) {
      return arr[idx];
    }

    inline UIElement *&operator[](u32 idx) const {
      return arr[idx];
    }

    inline u32 len() const {
      return size;
    }

    void resize(u32 newSize) {
      if (newSize < size) {
        // Destroy extra elements
        for (u32 i = newSize; i < size; i++) {
          if (arr[i] != nullptr) {
            delete arr[i];
            arr[i] = nullptr;
          }
        }
      }
      u32 loopEnd = newSize;
      if (size < newSize) {
        loopEnd = size;
      }
      UIElement **newArr = new UIElement *[newSize];
      for (u32 i = 0; i < loopEnd; i++) {
        newArr[i] = arr[i];
      }
      for (u32 i = loopEnd; i < newSize; i++) {
        newArr[i] = nullptr;
      }
      delete arr;
      arr = newArr;
      size = newSize;
    }
};

#endif //PRIME_PRACTICE_UIELEMENTLIST_HPP
