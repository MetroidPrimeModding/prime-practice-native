use crate::cpp_interface::text_renderer::{draw_text, is_pause_screen};
use crate::hooks::pause_screen::view::NavController;
use crate::{allocated_bytes, max_allocated_bytes};
use alloc::boxed::Box;
use spin::Mutex;
use view::View;

mod menu;
use crate::hooks::pause_screen::main_menu::create_main_menu;
pub use menu::*;

mod main_menu;
mod view;
mod warps;

const MENU_2_OFFSET_X: f32 = 170.0;
const MENU_3_OFFSET_X: f32 = 310.0;
const PAUSE_MENU_OFFSET: f32 = 50.0;

lazy_static! {
    static ref NAV_CONTROLLER: Mutex<NavController> =
        Mutex::new(NavController::new(Box::new(create_main_menu())));
}

pub fn draw() {
    let mem = allocated_bytes();
    let max = max_allocated_bytes();
    draw_text(&format!("dyn {}/max {}", mem, max), 10.0, 452.0);

    let nav = NAV_CONTROLLER.lock();
    nav.draw();
}

pub fn handle_input() {
    if is_pause_screen() {
        let mut nav = NAV_CONTROLLER.lock();
        // Don't care about the result
        nav.handle_input();
    }
}
