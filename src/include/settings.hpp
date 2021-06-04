#ifndef PRIME_PRACTICE_NATIVE_SETTINGS_HPP
#define PRIME_PRACTICE_NATIVE_SETTINGS_HPP

#include <gctypes.h>

struct Settings {
  bool OSD_show: 1{true};
  bool OSD_showFrameTime: 1{false};
  bool OSD_showMemoryInfo: 1{false};
  bool OSD_showMemoryGraph: 1{false};
  bool OSD_showInput: 1{true};
  bool OSD_showVelocity: 1{true};
  bool OSD_showPos: 1{true};
  bool OSD_showIGT: 1{true};
  bool OSD_showRoomTime: 1{true};

  bool TRIGGER_renderUnknown: 1{false};
  bool TRIGGER_renderLoad: 1{false};
  bool TRIGGER_renderDoor: 1{false};
  bool TRIGGER_renderForce: 1{false};
  bool TRIGGER_renderCameraHint: 1{false};

  s32 LAG_loop_iterations{0};
  s32 LAG_tri_renders{0};

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
