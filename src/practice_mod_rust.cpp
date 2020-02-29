#include <include/os.h>
#include <include/prime/CGraphics.hpp>
#include <include/NewPauseScreen.hpp>
#include "include/practice_mod_rust.h"
#include "include/TextRenderer.hpp"

void draw_text(const uint8_t *str, uint32_t len, float x, float y) {
  TextRenderer::RenderText((const char *) str, len, x, y);
}

void text_color(float r, float g, float b, float a) {
  TextRenderer::SetColor(r, g, b, a);
}

void rust_error(const uint8_t *fmt, uint32_t len) {
  char *res = new char[len + 1];;
  for (uint32_t i = 0; i < len; i++) {
    res[i] = fmt[i];
  }
  res[len] = '\0';
  OSReport(res);
  delete res;
}

float get_fps() {
  return CGraphics::GetFPS();
}

bool is_pause_screen() {
  return NewPauseScreen::instance->active;
}
