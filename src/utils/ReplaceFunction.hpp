
#pragma once

#include "os.h"
#include "types.h"
#include <functional>
#include <string.h>
#include <stdlib.h>
#include <type_traits>

template <typename T>
constexpr bool is_captureless_lambda = std::is_function_v<std::remove_pointer_t<decltype(+std::declval<T>())>>;

// trampoline pool
namespace ReplaceFunctionHookPool {
  /** Returns a pointer to the trampoline for the original function */
  void *Install(const char *name, void *originalPtr, void *replacementPtr);
  /** Called on OS reset */
  void Reset();

  template <typename FuncType, typename ReplacemntFunc>
  ReplacemntFunc InstallAtFn(const char *name, FuncType original, ReplacemntFunc replacement) {
    static_assert(is_captureless_lambda<ReplacemntFunc>,
                  "Replacement function must be a captureless lambda or function pointer");
    // Some spooky casting to convert possible member function pointers to void*
    union {
      FuncType mfp;
      void *addr;
    } u;
    u.mfp = original;
    void *originalPtr = u.addr;

    union {
      ReplacemntFunc mfp;
      void *addr;
    } u2;
    u2.mfp = replacement;
    void *replacementPtr = u2.addr;

    void *trampolinePtr = Install(name, originalPtr, replacementPtr);
    u2.addr = trampolinePtr;
    return u2.mfp;
  }
}; // namespace ReplaceFunctionHookPool

#define ALWAYS_INLINE inline __attribute__((always_inline))
#define _REPLACE_STATIC_CALLBACK_ASSERT()                                                                              \
  static_assert(!std::is_member_function_pointer_v<CallbackFuncPtr<>>, "Callback method must be static!")

template <typename T>
const char* PrettyName() {
  return __PRETTY_FUNCTION__;
}

template <typename T>
struct TypeName {
#if DEBUG
private:
  constexpr static char prefix[] = "const char *PrettyName() [T = ";
  constexpr static char suffix[] = "]";
  constexpr static size_t prefixLen = sizeof(prefix) - 1;
  constexpr static size_t suffixLen = sizeof(suffix) - 1;
public:
  char c_str[128]{};
  TypeName() {
    const char *prettyName = PrettyName<T>();
    size_t length = strlen(prettyName) - prefixLen - suffixLen + 1;
    if (length > sizeof(c_str)) {
      length = sizeof(c_str);
    }
    memcpy(c_str, prettyName + prefixLen, length - 1);
    c_str[length - 1] = '\0';
  }
#else
public:
  constexpr static const char* c_str = "Unk";
#endif
};


template <typename Derived>
class ReplacementHook {
  template <typename T = Derived>
  using CallbackFuncPtr = decltype(&T::Callback);

  static ALWAYS_INLINE auto &OrigRef() {
    _REPLACE_STATIC_CALLBACK_ASSERT();

    static constinit CallbackFuncPtr<> s_FnPtr = nullptr;
    return s_FnPtr;
  }

public:
  template <typename... Args>
  static ALWAYS_INLINE decltype(auto) Orig(Args &&...args) {
    _REPLACE_STATIC_CALLBACK_ASSERT();

    return OrigRef()(std::forward<Args>(args)...);
  }

  static ALWAYS_INLINE void InstallAtPtr(void *address) {
    _REPLACE_STATIC_CALLBACK_ASSERT();

    OrigRef() = ReplaceFunctionHookPool::Install(Derived::NAME, address, reinterpret_cast<void *>(Derived::Callback));
  }

  template <typename T>
  static ALWAYS_INLINE void InstallAtFuncPtr(T ptr) {
    _REPLACE_STATIC_CALLBACK_ASSERT();
    TypeName<Derived> typeName;
    OrigRef() = ReplaceFunctionHookPool::InstallAtFn(typeName.c_str, ptr, &Derived::Callback);
  }
};

#define DECLARE_FUNCTION_REPLACEMENT(name) struct name : ReplacementHook<name>

// // for some reason __COUNTER__ doesn't work inside macros without an extra layer
// #define CONCAT(a, b) CONCAT_INNER(a, b)
// #define CONCAT_INNER(a, b) a ## b
// #define REPLACE_FN(originalFn, replacementLambda) \
// ReplaceFunction CONCAT(replace_, __COUNTER__)(#originalFn, &originalFn, replacementLambda);
