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

extern bool is_pause_screen();

void on_frame();

void on_input();

extern float pad_a_a(uint32_t pad_index);

extern float pad_a_b(uint32_t pad_index);

extern float pad_a_d_down(uint32_t pad_index);

extern float pad_a_d_left(uint32_t pad_index);

extern float pad_a_d_right(uint32_t pad_index);

extern float pad_a_d_up(uint32_t pad_index);

extern float pad_a_l(uint32_t pad_index);

extern float pad_a_l_a_down(uint32_t pad_index);

extern float pad_a_l_a_left(uint32_t pad_index);

extern float pad_a_l_a_right(uint32_t pad_index);

extern float pad_a_l_a_up(uint32_t pad_index);

extern float pad_a_l_trigger(uint32_t pad_index);

extern float pad_a_r(uint32_t pad_index);

extern float pad_a_r_a_down(uint32_t pad_index);

extern float pad_a_r_a_left(uint32_t pad_index);

extern float pad_a_r_a_right(uint32_t pad_index);

extern float pad_a_r_a_up(uint32_t pad_index);

extern float pad_a_r_trigger(uint32_t pad_index);

extern float pad_a_start(uint32_t pad_index);

extern float pad_a_x(uint32_t pad_index);

extern float pad_a_y(uint32_t pad_index);

extern float pad_a_z(uint32_t pad_index);

extern bool pad_d_a(uint32_t pad_index);

extern bool pad_d_b(uint32_t pad_index);

extern bool pad_d_d_down(uint32_t pad_index);

extern bool pad_d_d_left(uint32_t pad_index);

extern bool pad_d_d_right(uint32_t pad_index);

extern bool pad_d_d_up(uint32_t pad_index);

extern bool pad_d_l(uint32_t pad_index);

extern bool pad_d_l_a_down(uint32_t pad_index);

extern bool pad_d_l_a_left(uint32_t pad_index);

extern bool pad_d_l_a_right(uint32_t pad_index);

extern bool pad_d_l_a_up(uint32_t pad_index);

extern bool pad_d_l_trigger(uint32_t pad_index);

extern bool pad_d_r(uint32_t pad_index);

extern bool pad_d_r_a_down(uint32_t pad_index);

extern bool pad_d_r_a_left(uint32_t pad_index);

extern bool pad_d_r_a_right(uint32_t pad_index);

extern bool pad_d_r_a_up(uint32_t pad_index);

extern bool pad_d_r_trigger(uint32_t pad_index);

extern bool pad_d_start(uint32_t pad_index);

extern bool pad_d_x(uint32_t pad_index);

extern bool pad_d_y(uint32_t pad_index);

extern bool pad_d_z(uint32_t pad_index);

extern bool pad_p_a(uint32_t pad_index);

extern bool pad_p_b(uint32_t pad_index);

extern bool pad_p_d_down(uint32_t pad_index);

extern bool pad_p_d_left(uint32_t pad_index);

extern bool pad_p_d_right(uint32_t pad_index);

extern bool pad_p_d_up(uint32_t pad_index);

extern bool pad_p_l(uint32_t pad_index);

extern bool pad_p_l_a_down(uint32_t pad_index);

extern bool pad_p_l_a_left(uint32_t pad_index);

extern bool pad_p_l_a_right(uint32_t pad_index);

extern bool pad_p_l_a_up(uint32_t pad_index);

extern bool pad_p_l_trigger(uint32_t pad_index);

extern bool pad_p_r(uint32_t pad_index);

extern bool pad_p_r_a_down(uint32_t pad_index);

extern bool pad_p_r_a_left(uint32_t pad_index);

extern bool pad_p_r_a_right(uint32_t pad_index);

extern bool pad_p_r_a_up(uint32_t pad_index);

extern bool pad_p_r_trigger(uint32_t pad_index);

extern bool pad_p_x(uint32_t pad_index);

extern bool pad_p_y(uint32_t pad_index);

extern bool pad_p_z(uint32_t pad_index);

extern bool pad_start(uint32_t pad_index);

extern void rust_error(const uint8_t *fmt, uint32_t len);

extern void text_color(float r, float g, float b, float a);

extern void warp(uint32_t world, uint32_t area);

} // extern "C"

#endif // PRACTICE_MOD_RUST_H
