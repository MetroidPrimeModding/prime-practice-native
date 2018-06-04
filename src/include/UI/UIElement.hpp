#ifndef PRIME_PRACTICE_UIELEMENT_HPP
#define PRIME_PRACTICE_UIELEMENT_HPP

#include "include/types.h"
#include "include/prime/CFinalInput.hpp"
//#include "include/UI/UIElementList.hpp"

#define kInvalidActiveChild (-1)

class UIElement {
private:
//    UIElementList children;
    s32 activeChild;
public:
    bool active;
    bool hidden;
    float x;
    float y;

    UIElement();
    UIElement(float x, float y);
    virtual ~UIElement();

    virtual void draw();
    virtual void handleInput(const CFinalInput &input);

    void addChild(UIElement *ele);
    void setActiveChild(s32 idx);
};

#endif //PRIME_PRACTICE_UIELEMENT_HPP
