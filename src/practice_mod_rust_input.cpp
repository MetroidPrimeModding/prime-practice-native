#include <include/os.h>
#include <include/prime/CGraphics.hpp>
#include <include/prime/CFinalInput.hpp>
#include <include/NewPauseScreen.hpp>
#include "include/TextRenderer.hpp"

//inline CFinalInput* getInput(uint32_t pad) {
//  // Will not be null when this is called!
//  // Quite a lot smaller to not nullcheck literally everywhere
//  NewPauseScreen *pauseScreen = NewPauseScreen::instance;
//  return &pauseScreen->inputs[pad % 4];
//}

#define getInput(pad) (&NewPauseScreen::instance->inputs[pad % 4])

//float pad_a_a(uint32_t pad_index) {
//  return getInput(pad_index)->AA();
//}
//float pad_a_b(uint32_t pad_index) {
//  return getInput(pad_index)->AB();
//}
//float pad_a_d_down(uint32_t pad_index) {
//  return getInput(pad_index)->ADPDown();
//}
//float pad_a_d_left(uint32_t pad_index) {
//  return getInput(pad_index)->ADPLeft();
//}
//float pad_a_d_right(uint32_t pad_index) {
//  return getInput(pad_index)->ADPRight();
//}
//float pad_a_d_up(uint32_t pad_index) {
//  return getInput(pad_index)->ADPUp();
//}
//float pad_a_l(uint32_t pad_index) {
//  return getInput(pad_index)->AL();
//}
float pad_a_l_a_down(uint32_t pad_index) {
  return getInput(pad_index)->ALADown();
}
float pad_a_l_a_left(uint32_t pad_index) {
  return getInput(pad_index)->ALALeft();
}
float pad_a_l_a_right(uint32_t pad_index) {
  return getInput(pad_index)->ALARight();
}
float pad_a_l_a_up(uint32_t pad_index) {
  return getInput(pad_index)->ALAUp();
}
float pad_a_l_trigger(uint32_t pad_index) {
  return getInput(pad_index)->ALTrigger();
}
float pad_a_r(uint32_t pad_index) {
  return getInput(pad_index)->AR();
}
float pad_a_r_a_down(uint32_t pad_index) {
  return getInput(pad_index)->ARADown();
}
float pad_a_r_a_left(uint32_t pad_index) {
  return getInput(pad_index)->ARALeft();
}
float pad_a_r_a_right(uint32_t pad_index) {
  return getInput(pad_index)->ARARight();
}
float pad_a_r_a_up(uint32_t pad_index) {
  return getInput(pad_index)->ARAUp();
}
float pad_a_r_trigger(uint32_t pad_index) {
  return getInput(pad_index)->ARTrigger();
}
//float pad_a_start(uint32_t pad_index) {
//  return getInput(pad_index)->AStart();
//}
//float pad_a_x(uint32_t pad_index) {
//  return getInput(pad_index)->AX();
//}
//float pad_a_y(uint32_t pad_index) {
//  return getInput(pad_index)->AY();
//}
//float pad_a_z(uint32_t pad_index) {
//  return getInput(pad_index)->AZ();
//}
bool pad_d_a(uint32_t pad_index) {
  return getInput(pad_index)->DA();
}
bool pad_d_r_a_down(uint32_t pad_index) {
  return getInput(pad_index)->DRADown();
}
bool pad_d_r_a_left(uint32_t pad_index) {
  return getInput(pad_index)->DRALeft();
}
bool pad_d_r_a_right(uint32_t pad_index) {
  return getInput(pad_index)->DRARight();
}
bool pad_d_r_a_up(uint32_t pad_index) {
  return getInput(pad_index)->DRAUp();
}
bool pad_d_l_a_down(uint32_t pad_index) {
  return getInput(pad_index)->DLADown();
}
bool pad_d_l_a_left(uint32_t pad_index) {
  return getInput(pad_index)->DLALeft();
}
bool pad_d_l_a_right(uint32_t pad_index) {
  return getInput(pad_index)->DLARight();
}
bool pad_d_l_a_up(uint32_t pad_index) {
  return getInput(pad_index)->DLAUp();
}
bool pad_d_b(uint32_t pad_index) {
  return getInput(pad_index)->DB();
}
bool pad_d_d_down(uint32_t pad_index) {
  return getInput(pad_index)->DDPDown();
}
bool pad_d_d_left(uint32_t pad_index) {
  return getInput(pad_index)->DDPLeft();
}
bool pad_d_d_right(uint32_t pad_index) {
  return getInput(pad_index)->DDPRight();
}
bool pad_d_d_up(uint32_t pad_index) {
  return getInput(pad_index)->DDPUp();
}
bool pad_d_l(uint32_t pad_index) {
  return getInput(pad_index)->DL();
}
bool pad_d_l_trigger(uint32_t pad_index) {
  return getInput(pad_index)->DLTrigger();
}
bool pad_d_r(uint32_t pad_index) {
  return getInput(pad_index)->DR();
}
bool pad_d_r_trigger(uint32_t pad_index) {
  return getInput(pad_index)->DRTrigger();
}
bool pad_d_start(uint32_t pad_index) {
  return getInput(pad_index)->DStart();
}
bool pad_d_x(uint32_t pad_index) {
  return getInput(pad_index)->DX();
}
bool pad_d_y(uint32_t pad_index) {
  return getInput(pad_index)->DY();
}
bool pad_d_z(uint32_t pad_index) {
  return getInput(pad_index)->DZ();
}
bool pad_p_a(uint32_t pad_index) {
  return getInput(pad_index)->PA();
}
//bool pad_p_l_a_down(uint32_t pad_index) {
//  return getInput(pad_index)->PLADown();
//}
//bool pad_p_l_a_left(uint32_t pad_index) {
//  return getInput(pad_index)->PLALeft();
//}
//bool pad_p_l_a_right(uint32_t pad_index) {
//  return getInput(pad_index)->PLARight();
//}
//bool pad_p_l_a_up(uint32_t pad_index) {
//  return getInput(pad_index)->PLAUp();
//}
//bool pad_p_r_a_down(uint32_t pad_index) {
//  return getInput(pad_index)->PRADown();
//}
//bool pad_p_r_a_left(uint32_t pad_index) {
//  return getInput(pad_index)->PRALeft();
//}
//bool pad_p_r_a_right(uint32_t pad_index) {
//  return getInput(pad_index)->PRARight();
//}
//bool pad_p_r_a_up(uint32_t pad_index) {
//  return getInput(pad_index)->PRAUp();
//}
bool pad_p_b(uint32_t pad_index) {
  return getInput(pad_index)->PB();
}
//bool pad_p_d_down(uint32_t pad_index) {
//  return getInput(pad_index)->PDPDown();
//}
//bool pad_p_d_left(uint32_t pad_index) {
//  return getInput(pad_index)->PDPLeft();
//}
//bool pad_p_d_right(uint32_t pad_index) {
//  return getInput(pad_index)->PDPRight();
//}
//bool pad_p_d_up(uint32_t pad_index) {
//  return getInput(pad_index)->PDPUp();
//}
//bool pad_p_l(uint32_t pad_index) {
//  return getInput(pad_index)->PL();
//}
//bool pad_p_l_trigger(uint32_t pad_index) {
//  return getInput(pad_index)->PLTrigger();
//}
//bool pad_p_r(uint32_t pad_index) {
//  return getInput(pad_index)->PR();
//}
//bool pad_p_r_trigger(uint32_t pad_index) {
//  return getInput(pad_index)->PRTrigger();
//}
//bool pad_p_x(uint32_t pad_index) {
//  return getInput(pad_index)->PX();
//}
//bool pad_p_y(uint32_t pad_index) {
//  return getInput(pad_index)->PY();
//}
//bool pad_p_z(uint32_t pad_index) {
//  return getInput(pad_index)->PZ();
//}
bool pad_start(uint32_t pad_index) {
  return getInput(pad_index)->PStart();
}