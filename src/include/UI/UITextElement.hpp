#ifndef PRIME_PRACTICE_UITEXTELEMENT_HPP
#define PRIME_PRACTICE_UITEXTELEMENT_HPP

#include "include/UI/UIElement.hpp"

class UITextElement : public UIElement {
    char* str;
public:
    UITextElement(const char* text, float x, float y);
    virtual ~UITextElement();

    virtual void draw();
    void setText(const char* text);
};

#endif //PRIME_PRACTICE_UITEXTELEMENT_HPP
