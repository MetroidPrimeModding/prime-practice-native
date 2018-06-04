#ifndef PRIME_PRACTICE_UIHEAPUSAGEELEMENT_HPP
#define PRIME_PRACTICE_UIHEAPUSAGEELEMENT_HPP

#include "include/UI/UIElement.hpp"

class UIHeapUsageElement : public UIElement {
public:
    UIHeapUsageElement(float x, float y) : peakPercent(0), minFree(0x1800000), UIElement(x, y) {};
    virtual void draw();

    u32 peakPercent;
    u32 minFree;
};

#endif //PRIME_PRACTICE_UIHEAPUSAGEELEMENT_HPP
