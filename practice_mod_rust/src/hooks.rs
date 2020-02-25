use alloc::string::String;

use libm;

use crate::cpp_interface::text_renderer::{draw_text, set_text_color};
use crate::cpp_interface::{TextLocation, CONFIG};
use crate::globals;

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
            draw_igt(pos);
            pos.x += 110.0;
        }
        pos.y += LINE_HEIGHT;
        pos.x = 10.0;
    }
    if CONFIG.show_speed {
        draw_player_speed(pos);
        pos.y += LINE_HEIGHT;
    }
}

#[no_mangle]
pub extern "C" fn on_input() {}

fn draw_igt(pos: TextLocation) -> Option<()> {
    let state = globals::global_objects().gameState()?;
    let time = state.playtime()?.value()?;

    let ms = ((time * 1000.0) % 1000.0) as u32;
    let sec = (time % 60.0) as u32;
    let minutes = ((time / 60.0) % 60.0) as u32;
    let hours = ((time / 60.0 / 60.0) % 60.0) as u32;

    let text = format!("{:02}:{:02}:{:02}.{:03}", hours, minutes, sec, ms);

    draw_text(&text, pos);
    Some(())
}

fn draw_player_speed(pos: TextLocation) -> Option<()> {
    let player = globals::state_manager().player()?;

    let velocity = player.sup()?.velocity()?;
    let (x, y, z) = (
        velocity.x()?.value()?,
        velocity.y()?.value()?,
        velocity.z()?.value()?,
    );
    let angularVelocity = player.sup()?.angularVelocity()?;

    let (rx, ry, rz) = (
        angularVelocity.x()?.value()?,
        angularVelocity.y()?.value()?,
        angularVelocity.z()?.value()?,
    );

    let hspeed = libm::sqrtf(x * x + y * y);
    let rspeed = libm::sqrtf(rx * rx + ry * ry + rz * rz);

    let text = format!(
        "s {:7.2}x {:7.2}y {:7.2}z {:7.2}h r {:7.2}x {:7.2}y {:7.2}z {:7.2}t",
        x, y, z, hspeed, rx, ry, rz, rspeed
    );

    draw_text(&text, pos);

    Some(())
}
