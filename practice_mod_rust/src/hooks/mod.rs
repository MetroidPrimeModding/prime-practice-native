use crate::cpp_interface::text_renderer::{is_pause_screen, set_text_color};
use crate::cpp_interface::{TextLocation, CONFIG};

const CHAR_DIM: f32 = 8.0;
const LINE_PADDING: f32 = 2.0;
const LINE_HEIGHT: f32 = CHAR_DIM + LINE_PADDING;

#[no_mangle]
pub extern "C" fn on_frame() {
    let mut pos = TextLocation { x: 10.0, y: 10.0 };
    set_text_color(1.0, 1.0, 1.0, 1.0);
    if CONFIG.show_igt || CONFIG.show_input {
        // draw_text("Prime Practice Mod", pos);
        if CONFIG.show_igt {
            osd::draw_igt(pos);
            pos.x += 110.0;
        }
        pos.y += LINE_HEIGHT;
        pos.x = 10.0;
        //TODO: input
    }
    if CONFIG.show_speed {
        osd::draw_player_speed(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_pos {
        osd::draw_player_pos(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_high_p_pos {
        osd::draw_player_high_p_pos(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_room_timers {
        osd::draw_room_timers(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_fps {
        osd::draw_fps(TextLocation { x: 610.0, y: 452.0 });
    }
    if is_pause_screen() {
        pause_screen::draw();
    }
}

#[no_mangle]
pub extern "C" fn on_input() {}

mod osd;
mod pause_screen;
