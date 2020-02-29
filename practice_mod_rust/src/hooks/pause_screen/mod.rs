use crate::cpp_interface::text_renderer::draw_text;
use crate::{allocated_bytes, max_allocated_bytes};
use alloc::string::String;

pub fn draw() {
    let mem = allocated_bytes();
    let max = max_allocated_bytes();
    let mut text = format!("dyn {}/max {}", mem, max);
    draw_text(&text, 10.0, 452.0);
}

mod menu;
pub use menu::*;
