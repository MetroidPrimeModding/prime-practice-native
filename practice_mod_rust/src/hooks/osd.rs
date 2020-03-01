use crate::cpp_interface::c_interface;
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
    let (px, py, pz) = (
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

    let hspeed = libm::sqrtf(px * px + py * py);
    let rspeed = libm::sqrtf(rx * rx + ry * ry + rz * rz);

    let text = format!(
        "s {:7.2}x {:7.2}y {:7.2}z {:7.2}h r {:7.2}x {:7.2}y {:7.2}z {:7.2}t",
        px, py, pz, hspeed, rx, ry, rz, rspeed
    );

    draw_text(&text, x, y);

    Some(())
}

pub fn draw_player_pos(x: f32, y: f32) -> Option<()> {
    let player = globals::state_manager().player()?.deref()?;

    let player_pos = player.sup()?.translation()?;
    let (px, py, pz) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.2}x {:7.2}y {:7.2}z", px, py, pz);

    draw_text(&text, x, y);

    Some(())
}

pub fn draw_player_high_p_pos(x: f32, y: f32) -> Option<()> {
    let player = globals::state_manager().player()?.deref()?;

    let player_pos = player.sup()?.translation()?;
    let (px, py, pz) = (
        player_pos.x()?.value()?,
        player_pos.y()?.value()?,
        player_pos.z()?.value()?,
    );

    let text = format!("p {:7.7}x {:7.7}y {:7.7}z", px, py, pz);

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

fn if_bool(if_true: char, if_false: char, value: bool) -> char {
    if value {
        if_true
    } else {
        if_false
    }
}

pub fn draw_input(x: f32, y: f32) -> Option<()> {
    let mut text = String::with_capacity(38);
    unsafe {
        text.push(if_bool('\x18', ' ', c_interface::pad_d_l_a_up(0)));
        text.push(if_bool('\x19', ' ', c_interface::pad_d_l_a_down(0)));
        text.push(if_bool('\x1B', ' ', c_interface::pad_d_l_a_left(0)));
        text.push(if_bool('\x1A', ' ', c_interface::pad_d_l_a_right(0)));

        text.push(if_bool('a', ' ', c_interface::pad_d_a(0)));
        text.push(if_bool('b', ' ', c_interface::pad_d_b(0)));
        text.push(if_bool('x', ' ', c_interface::pad_d_x(0)));
        text.push(if_bool('y', ' ', c_interface::pad_d_y(0)));
        text.push(if_bool('s', ' ', c_interface::pad_d_start(0)));
        text.push(if_bool('z', ' ', c_interface::pad_d_z(0)));

        text.push(if_bool('l', ' ', c_interface::pad_d_l(0)));
        text.push(if_bool('L', ' ', c_interface::pad_d_l_trigger(0)));
        text.push(if_bool('r', ' ', c_interface::pad_d_r(0)));
        text.push(if_bool('R', ' ', c_interface::pad_d_r_trigger(0)));

        text.push(if_bool(
            'd',
            ' ',
            c_interface::pad_d_d_up(0)
                || c_interface::pad_d_d_down(0)
                || c_interface::pad_d_d_left(0)
                || c_interface::pad_d_d_right(0),
        ));

        text.push(if_bool('\x18', ' ', c_interface::pad_d_d_up(0)));
        text.push(if_bool('\x19', ' ', c_interface::pad_d_d_down(0)));
        text.push(if_bool('\x1B', ' ', c_interface::pad_d_d_left(0)));
        text.push(if_bool('\x1A', ' ', c_interface::pad_d_d_right(0)));

        text.push(if_bool(
            'c',
            ' ',
            c_interface::pad_d_r_a_up(0)
                || c_interface::pad_d_r_a_down(0)
                || c_interface::pad_d_r_a_left(0)
                || c_interface::pad_d_r_a_right(0),
        ));

        text.push(if_bool('\x18', ' ', c_interface::pad_d_r_a_up(0)));
        text.push(if_bool('\x19', ' ', c_interface::pad_d_r_a_down(0)));
        text.push(if_bool('\x1B', ' ', c_interface::pad_d_r_a_left(0)));
        text.push(if_bool('\x1A', ' ', c_interface::pad_d_r_a_right(0)));

        let stick_x = c_interface::pad_a_l_a_right(0) - c_interface::pad_a_l_a_left(0);
        let stick_y = c_interface::pad_a_l_a_up(0) - c_interface::pad_a_l_a_down(0);

        text.push_str(&format!(
            "{:6.3}{:6.3}{:6.3}",
            stick_x,
            stick_y,
            libm::sqrtf(stick_x * stick_x + stick_y * stick_y)
        ))
    }

    draw_text(&text, x, y);
    Some(())
}
