mod c_interface {
    extern "C" {
        #[no_mangle]
        pub fn draw_text(str: *const u8, len: u32, x: f32, y: f32);
        #[no_mangle]
        pub fn text_color(r: f32, g: f32, b: f32, a: f32);
    }
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct ModConfig {
    pub show_speed: bool,
    pub show_pos: bool,
    pub show_high_p_pos: bool,
    pub show_room_timers: bool,
    pub show_igt: bool,
    pub show_input: bool,
    pub show_fps: bool,
    pub show_unknown_triggers: bool,
    pub show_load_triggers: bool,
    pub show_door_triggers: bool,
    pub show_force_triggers: bool,
    pub show_camera_hint_triggers: bool,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct TextLocation {
    pub x: f32,
    pub y: f32,
}

impl TextLocation {
    pub fn from(x: impl Into<f32> + Copy, y: impl Into<f32> + Copy) -> Self {
        return TextLocation {
            x: x.into(),
            y: y.into(),
        };
    }
}

pub static CONFIG: ModConfig = ModConfig {
    show_speed: true,
    show_pos: false,
    show_high_p_pos: false,
    show_room_timers: false,
    show_igt: true,
    show_input: true,
    show_fps: true,
    show_unknown_triggers: false,
    show_load_triggers: false,
    show_door_triggers: false,
    show_force_triggers: false,
    show_camera_hint_triggers: false,
};

pub mod text_renderer {
    use super::c_interface;
    use crate::cpp_interface::TextLocation;

    pub fn set_text_color(r: f32, g: f32, b: f32, a: f32) {
        unsafe { c_interface::text_color(r, g, b, a) }
    }

    pub fn draw_text(str: &str, loc: TextLocation) {
        unsafe { c_interface::draw_text(str.as_ptr(), str.len() as u32, loc.x, loc.y) }
    }
}
