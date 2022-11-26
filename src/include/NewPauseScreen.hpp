#ifndef NEW_PAUSE_SCREEN_HPP
#define NEW_PAUSE_SCREEN_HPP

#include <prime/CScriptTrigger.hpp>
#include <prime/CFinalInput.hpp>
#include <imgui.h>
#include "prime/CStateManager.hpp"
#include "utils.hpp"

void warp(uint32_t world, uint32_t area);

class NewPauseScreen {
public:
  static NewPauseScreen *instance;
  bool pauseScreenActive;
  bool menuActive;
  CFinalInput *inputs;

  NewPauseScreen();

  void Render();
  void HandleInputs();
  void update(float d) const;

  void hide();
  void show();
private:
  void RenderMenu();
  float hotkeyInputTimeout = 0;
  int upPresses = 0;
  int downPresses = 0;
};


#endif
