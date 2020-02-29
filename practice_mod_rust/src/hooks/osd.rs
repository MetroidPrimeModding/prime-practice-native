use crate::cpp_interface::text_renderer::{draw_text, get_fps};
use crate::globals;
use alloc::string::String;

pub fn draw_igt(x: f32, y: f32) -> Option<()> {
    let state = globals::global_objects().gameState()?.deref()?;
    let time = state.playtime()?.value()?;

    let ms = ((time * 1000.0) % 1000.0) as u32;
    let sec = (time % 60.0) as u32;
    let minutes = ((time / 60.0) % 60.0) as u32;
    let hours = ((time / 60.0 / 60.0) % 60.0) as u32;

    let text = format!("{:02}:{:02}:{:02}.{:03}", hours, minutes, sec, ms);

    draw_text(&text, x, y);
    Some(())
}

pub fn draw_player_speed(x: f32, y: f32) -> Option<()> {
    let player = globals::state_manager().player()?.deref()?;

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

    draw_text(&text, x, y);

    Some(())
}

pub fn draw_player_pos(x: f32, y: f32) -> Option<()> {
    let player = globals::state_manager().player()?.deref()?;

    let player_pos = player.sup()?.translation()?;
    let (x, y, z) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.2}x {:7.2}y {:7.2}z", x, y, z);

    draw_text(&text, x, y);

    Some(())
}

pub fn draw_player_high_p_pos(x: f32, y: f32) -> Option<()> {
    let player = globals::state_manager().player()?.deref()?;

    let player_pos = player.sup()?.translation()?;
    let (x, y, z) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.7}x {:7.7}y {:7.7}z", x, y, z);

    draw_text(&text, x, y);

    Some(())
}

pub fn draw_fps(x: f32, y: f32) -> Option<()> {
    let fps = get_fps() as u32;

    let text = format!("{}", fps);
    draw_text(&text, x, y);

    Some(())
}

static mut CURRENT_ROOM: u32 = 0;
static mut LAST_ROOM_TIME: f64 = 0.0;
static mut CURRENT_ROOM_START: f64 = 0.0;

pub fn draw_room_timers(x: f32, y: f32) -> Option<()> {
    fn time_to_frames(v: f64) -> u32 {
        (v * 60.0) as u32
    }

    let time = globals::global_objects()
        .gameState()?
        .deref()?
        .playtime()?
        .value()?;
    let room = globals::state_manager()
        .world()?
        .deref()?
        .currentAreaID()?
        .value()?;

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

    draw_text(&text, x, y);

    Some(())
}
