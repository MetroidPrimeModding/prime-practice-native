#pragma once

#include <gctypes.h>

#ifdef DEBUG
#define DEBUG_TRUE true
#else
#define DEBUG_TRUE false
#endif

struct Settings {
  bool OSD_show : 1 {true};
  bool OSD_showFrameTime : 1 {false};
  bool OSD_showMemoryInfo : 1 {false};
  bool OSD_showMemoryGraph : 1 {false};
  bool OSD_showInput : 1 {true};
  bool OSD_showVelocity : 1 {true};
  bool OSD_showRotationalVelocity : 1 {false};
  bool OSD_showPos : 1 {true};
  bool OSD_showIGT : 1 {true};
  bool OSD_showCurrentRoomTime : 1 {true};
  bool OSD_showPreviousRoomTime : 1 {true};
  bool OSD_showMostRecentDoorToLoadTime : 1 {false};
  bool OSD_showLoads : 1 {DEBUG_TRUE};
  bool OSD_showRng : 1 {false};
  bool OSD_showIDrone : 1 {true};
  bool OSD_showTargetInfo : 1 {false};
  bool OSD_showJumpState : 1 {false};

  bool BOMBJUMP_enable : 1 {false};
  bool BOMBJUMP_infiniteBombs : 1 {false};

  bool TRIGGER_renderUnknown : 1 {false};
  bool TRIGGER_renderLoad : 1 {false};
  bool TRIGGER_renderDoor : 1 {false};
  bool TRIGGER_renderForce : 1 {false};
  bool TRIGGER_renderCameraHint : 1 {false};

  s32 LAG_loop_iterations{0};
  s32 LAG_tri_renders{0};
  bool RNG_lockSeed{false};
  bool SCAN_infiniteScanTime{false};
  bool SCAN_infiniteScanTimeOnImportantScans{false};

  inline bool TRIGGER_anyOn() {
    return TRIGGER_renderUnknown || TRIGGER_renderLoad || TRIGGER_renderDoor || TRIGGER_renderForce ||
           TRIGGER_renderCameraHint;
  }
};

// Saved to the memory card by id (see CardGate). Ids are permanent: never reuse or renumber one, only append.
#define SETTINGS_FIELDS(X) \
  X(1, OSD_show) \
  X(2, OSD_showFrameTime) \
  X(3, OSD_showMemoryInfo) \
  X(4, OSD_showMemoryGraph) \
  X(5, OSD_showInput) \
  X(6, OSD_showVelocity) \
  X(7, OSD_showRotationalVelocity) \
  X(8, OSD_showPos) \
  X(9, OSD_showIGT) \
  X(10, OSD_showCurrentRoomTime) \
  X(11, OSD_showPreviousRoomTime) \
  X(12, OSD_showMostRecentDoorToLoadTime) \
  X(13, OSD_showLoads) \
  X(14, OSD_showRng) \
  X(15, OSD_showIDrone) \
  X(16, OSD_showTargetInfo) \
  X(17, OSD_showJumpState) \
  X(18, BOMBJUMP_enable) \
  X(19, BOMBJUMP_infiniteBombs) \
  X(20, TRIGGER_renderUnknown) \
  X(21, TRIGGER_renderLoad) \
  X(22, TRIGGER_renderDoor) \
  X(23, TRIGGER_renderForce) \
  X(24, TRIGGER_renderCameraHint) \
  X(25, LAG_loop_iterations) \
  X(26, LAG_tri_renders) \
  X(27, RNG_lockSeed) \
  X(28, SCAN_infiniteScanTime) \
  X(29, SCAN_infiniteScanTimeOnImportantScans)

extern Settings SETTINGS;
