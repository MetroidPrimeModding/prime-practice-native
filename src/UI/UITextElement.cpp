#include "include/UI/UITextElement.hpp"
#include "include/TextRenderer.hpp"
#include <string.h>

UITextElement::UITextElement(const char *text, float x, float y): UIElement(x, y) {
  u32 len = strlen(text);
  this->str = new char[len];
  memcpy(this->str, text, len);
}

UITextElement::~UITextElement() {
  delete this->str;
}

void UITextElement::draw() {
  if (hidden) return;
  UIElement::draw();
  TextRenderer::RenderText(str, x, y);
}

void UITextElement::setText(const char *text) {
  delete this->str;
  u32 len = strlen(text);
  this->str = new char[len];
  memcpy(this->str, text, len);
}
