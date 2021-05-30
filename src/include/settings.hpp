#ifndef PRIME_PRACTICE_NATIVE_SETTINGS_HPP
#define PRIME_PRACTICE_NATIVE_SETTINGS_HPP

struct Settings {
  bool showInGame : 1{true};
  bool showFrameTime : 1{false};
  bool showMemoryInfo : 1{false};
  bool showMemoryGraph : 1{false};
  bool showInput: 1{true};
};

extern Settings SETTINGS;

#endif //PRIME_PRACTICE_NATIVE_SETTINGS_HPP
