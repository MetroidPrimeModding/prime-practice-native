#ifndef PRACTICE_MOD_RUST_H
#define PRACTICE_MOD_RUST_H

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <new>

struct ModConfig {
  bool show_speed;
  bool show_pos;
  bool show_high_p_pos;
  bool show_room_timers;
  bool show_igt;
  bool show_input;
  bool show_fps;
  bool show_unknown_triggers;
  bool show_load_triggers;
  bool show_door_triggers;
  bool show_force_triggers;
  bool show_camera_hint_triggers;
};

extern "C" {

extern void draw_text(const uint8_t *str, uint32_t len, float x, float y);

extern float get_fps();

const uint8_t *hello();

void on_frame();

void on_input();

extern void rust_error(const uint8_t *fmt, uint32_t len);

extern void text_color(float r, float g, float b, float a);

} // extern "C"

#endif // PRACTICE_MOD_RUST_H
