#ifndef NEW_PAUSE_SCREEN_HPP
#define NEW_PAUSE_SCREEN_HPP

#include <prime/CScriptTrigger.hpp>
#include <prime/CFinalInput.hpp>
#include "prime/CStateManager.hpp"
#include "STriggerRenderConfig.hpp"

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

  void RenderWorld();
  void show();
private:
  void drawTrigger(const STriggerRenderConfig &trigger, CObjectList *pTrigger, CScriptTrigger *pScriptTrigger) const;
  ETriggerType determineTriggerType(CObjectList *pTrigger, CScriptTrigger *pScriptTrigger) const;

  // TODO: vector?
//    static constexpr int MENU_MAX = 32;
//    Menu *menuStack[MENU_MAX];
//    int topMenu = 0;

  void RenderMenu();
  void InitIMGui();
  void ImGuiNewFrame();
};

#endif
