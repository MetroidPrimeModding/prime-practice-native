#include "include/UI/UIElement.hpp"
#include "include/TextRenderer.hpp"

UIElement::UIElement(): x(0), y(0) {
  this->activeChild = kInvalidActiveChild;
  active = true;
  hidden = false;
}

UIElement::~UIElement() {
}

void UIElement::draw() {
  if (hidden) return;

//  for (u32 i = 0; i < children.len(); i++) {
//    if (!children[i]->hidden) {
//      children[i]->draw();
//    }
//  }
}

void UIElement::handleInput(const CFinalInput &input) {
  if (activeChild != kInvalidActiveChild) {
//    children[activeChild]->handleInput(input);
  }
}

void UIElement::addChild(UIElement *ele) {
//  children.add(ele);
}

void UIElement::setActiveChild(s32 idx) {
  if (activeChild != kInvalidActiveChild) {
//    children[activeChild]->active = false;
  }
  activeChild = idx;
  if (activeChild != kInvalidActiveChild) {
//    children[activeChild]->active = true;
  }
}

UIElement::UIElement(float x, float y): x(x), y(y){

}
