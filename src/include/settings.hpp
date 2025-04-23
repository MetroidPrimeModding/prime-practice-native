#ifndef PRIME_PRACTICE_NATIVE_SETTINGS_HPP
#define PRIME_PRACTICE_NATIVE_SETTINGS_HPP

#include <gctypes.h>

#ifdef DEBUG
#define DEBUG_TRUE true
#else
#define DEBUG_TRUE false
#endif

struct Settings {
  bool OSD_show: 1{true};
  bool OSD_showFrameTime: 1{false};
  bool OSD_showMemoryInfo: 1{false};
  bool OSD_showMemoryGraph: 1{false};
  bool OSD_showInput: 1{true};
  bool OSD_showVelocity: 1{true};
  bool OSD_showRotationalVelocity: 1{false};
  bool OSD_showPos: 1{true};
  bool OSD_showIGT: 1{true};
  bool OSD_showCurrentRoomTime: 1{true};
  bool OSD_showPreviousRoomTime: 1{true};
  bool OSD_showLoads: 1{DEBUG_TRUE};
  bool OSD_showRng: 1{false};
  bool OSD_showIDrone: 1{true};

  bool BOMBJUMP_enable: 1{false};
  bool BOMBJUMP_infiniteBombs: 1{false};

  bool TRIGGER_renderUnknown: 1{false};
  bool TRIGGER_renderLoad: 1{false};
  bool TRIGGER_renderDoor: 1{false};
  bool TRIGGER_renderForce: 1{false};
  bool TRIGGER_renderCameraHint: 1{false};

  s32 LAG_loop_iterations{0};
  s32 LAG_tri_renders{0};
  bool RNG_lockSeed{false};

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
