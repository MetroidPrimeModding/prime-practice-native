#ifndef NEW_PAUSE_SCREEN_HPP
#define NEW_PAUSE_SCREEN_HPP

#include <include/prime/CScriptTrigger.hpp>
#include <include/prime/CFinalInput.hpp>
#include <UI/Menu.h>
#include "include/prime/CStateManager.h"
#include "include/STriggerRenderConfig.hpp"

void warp(uint32_t world, uint32_t area);

class NewPauseScreen {
public:
    static NewPauseScreen *instance;
    bool active;
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

    Menu *currentMenu;
    void RenderMenu();
};

#endif
