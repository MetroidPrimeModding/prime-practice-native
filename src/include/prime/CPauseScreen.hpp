#ifndef PRIMEWATCH_PRIME_CPAUSESCREEN_HPP
#define PRIMEWATCH_PRIME_CPAUSESCREEN_HPP

class CPauseScreen {
public:
    typedef enum ESubScreen {
        ESubScreen_LogBook,
        ESubScreen_Options,
        ESubScreen_Inventory,
        ESubScreen_ToGame,
        ESubScreen_ToMap
    } ESubScreen;

    ESubScreen x0_initialSubScreen;
    u32 x4_;
    ESubScreen x8_curSubscreen;
    ESubScreen xc_nextSubscreen;
    float x10_alphaInterp;

    void StartTransition(float time, const CStateManager &mgr, ESubScreen subscreen, int);
    bool CheckLoadComplete(const CStateManager &mgr);
    void InitializeFrameGlue();
    bool InputEnabled() const;
    static ESubScreen GetPreviousSubscreen(ESubScreen screen);
    static ESubScreen GetNextSubscreen(ESubScreen screen);
    void TransitionComplete();

    void ProcessControllerInput(const CStateManager &mgr, const CFinalInput &input);
    void PreDraw();
    void Draw();
    bool IsLoaded() const;
};

#endif //PRIMEWATCH_PRIME_CPAUSESCREEN_HPP
