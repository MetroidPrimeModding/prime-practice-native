use crate::cpp_interface::text_renderer::{is_pause_screen, set_text_color};
use crate::cpp_interface::CONFIG;

pub const CHAR_DIM: f32 = 8.0;
pub const LINE_PADDING: f32 = 2.0;
pub const LINE_HEIGHT: f32 = CHAR_DIM + LINE_PADDING;

#[no_mangle]
pub extern "C" fn on_frame() {
    let mut x = 10.0f32;
    let mut y = 10.0f32;
    set_text_color(1.0, 1.0, 1.0, 1.0);
    if CONFIG.show_igt || CONFIG.show_input {
        // draw_text("Prime Practice Mod", pos);
        if CONFIG.show_igt {
            osd::draw_igt(x + 110.0, y);
        }
        y += LINE_HEIGHT;
        //TODO: input
    }
    if CONFIG.show_speed {
        osd::draw_player_speed(x, y);
        y += LINE_HEIGHT;
    }
    if CONFIG.show_pos {
        osd::draw_player_pos(x, y);
        y += LINE_HEIGHT;
    }
    if CONFIG.show_high_p_pos {
        osd::draw_player_high_p_pos(x, y);
        y += LINE_HEIGHT;
    }
    if CONFIG.show_room_timers {
        osd::draw_room_timers(x, y);
        y += LINE_HEIGHT;
    }
    if CONFIG.show_fps {
        osd::draw_fps(610.0, 452.0);
    }
    if is_pause_screen() {
        pause_screen::draw();
    }
}

#[no_mangle]
pub extern "C" fn on_input() {}

mod osd;
mod pause_screen;
