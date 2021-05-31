#ifndef PRIME_PRACTICE_NATIVE_SETTINGS_HPP
#define PRIME_PRACTICE_NATIVE_SETTINGS_HPP

struct Settings {
  bool OSD_show: 1{true};
  bool OSD_showFrameTime: 1{false};
  bool OSD_showMemoryInfo: 1{false};
  bool OSD_showMemoryGraph: 1{false};
  bool OSD_showInput: 1{true};

  bool TRIGGER_renderUnknown: 1{false};
  bool TRIGGER_renderLoad: 1{false};
  bool TRIGGER_renderDoor: 1{false};
  bool TRIGGER_renderForce: 1{false};
  bool TRIGGER_renderCameraHint: 1{false};

  inline bool TRIGGER_anyOn() {
    return TRIGGER_renderUnknown
           || TRIGGER_renderLoad
           || TRIGGER_renderDoor
           || TRIGGER_renderForce
           || TRIGGER_renderCameraHint;
  }
};

extern Settings SETTINGS;

#endif //PRIME_PRACTICE_NATIVE_SETTINGS_HPP
