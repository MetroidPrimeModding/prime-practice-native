use alloc::string::String;

use libm;

use crate::cpp_interface::text_renderer::{draw_text, get_fps, set_text_color};
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
        //TODO: input
    }
    if CONFIG.show_speed {
        draw_player_speed(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_pos {
        draw_player_pos(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_high_p_pos {
        draw_player_high_p_pos(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_room_timers {
        draw_room_timers(pos);
        pos.y += LINE_HEIGHT;
    }
    if CONFIG.show_fps {
        draw_fps(TextLocation { x: 610.0, y: 452.0 });
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
    let angular_velocity = player.sup()?.angularVelocity()?;
    let (rx, ry, rz) = (
        angular_velocity.x()?.value()?,
        angular_velocity.y()?.value()?,
        angular_velocity.z()?.value()?,
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

fn draw_player_pos(pos: TextLocation) -> Option<()> {
    let player = globals::state_manager().player()?;

    let player_pos = player.sup()?.translation()?;
    let (x, y, z) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.2}x {:7.2}y {:7.2}z", x, y, z);

    draw_text(&text, pos);

    Some(())
}

fn draw_player_high_p_pos(pos: TextLocation) -> Option<()> {
    let player = globals::state_manager().player()?;

    let player_pos = player.sup()?.translation()?;
    let (x, y, z) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.7}x {:7.7}y {:7.7}z", x, y, z);

    draw_text(&text, pos);

    Some(())
}

fn draw_fps(pos: TextLocation) -> Option<()> {
    let fps = get_fps() as u32;

    let text = format!("{}", fps);
    draw_text(&text, pos);

    Some(())
}

static mut CURRENT_ROOM: u32 = 0;
static mut LAST_ROOM_TIME: f64 = 0.0;
static mut CURRENT_ROOM_START: f64 = 0.0;

fn draw_room_timers(pos: TextLocation) -> Option<()> {
    fn time_to_frames(v: f64) -> u32 {
        (v * 60.0) as u32
    }

    let fps = get_fps() as u32;

    let time = globals::global_objects().gameState()?.playtime()?.value()?;
    let room = globals::state_manager().world()?.currentAreaID()?.value()?;

    let text: String;
    unsafe {
        // static variables are safe here, since this is single-threaded access
        if room != CURRENT_ROOM {
            CURRENT_ROOM = room;
            LAST_ROOM_TIME = time - CURRENT_ROOM_START;
            CURRENT_ROOM_START = time;
        }
        let current_room_time = time - CURRENT_ROOM_START;
        text = format!(
            "Room Time: {:8.3}/{:6} | Last Room: {:8.3}/{:6}",
            current_room_time,
            time_to_frames(current_room_time),
            LAST_ROOM_TIME,
            time_to_frames(LAST_ROOM_TIME)
        );
    }

    draw_text(&text, pos);

    Some(())
}
