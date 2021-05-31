#ifndef NEW_PAUSE_SCREEN_HPP
#define NEW_PAUSE_SCREEN_HPP

#include <prime/CScriptTrigger.hpp>
#include <prime/CFinalInput.hpp>
#include "prime/CStateManager.hpp"

void warp(uint32_t world, uint32_t area);

class NewPauseScreen {
public:
  static NewPauseScreen *instance;
  bool pauseScreenActive;
  bool menuActive;
  int frames;
  CFinalInput *inputs;

  NewPauseScreen();

  void Render();

  void HandleInputs();

  bool shouldRenderGloballyInsteadOfInWorld();

  void hide();

  void show();
private:
  void RenderMenu();
  void InitIMGui_BundledFont();
  void InitIMGui_GenerateFont();
  void ImGuiNewFrame();
};

#endif
