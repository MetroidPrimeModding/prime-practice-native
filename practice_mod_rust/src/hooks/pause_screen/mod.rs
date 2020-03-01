use crate::cpp_interface::text_renderer::{draw_text, is_pause_screen};
use crate::hooks::pause_screen::OnSelectResult::{Deslelect, DoNothing};
use crate::{allocated_bytes, max_allocated_bytes, MPStdout};
use alloc::boxed::Box;
use core::fmt::Write;
pub use menu::*;
use spin::Mutex;

mod menu;

const MENU_2_OFFSET_X: f32 = 170.0;
const MENU_3_OFFSET_X: f32 = 310.0;
const PAUSE_MENU_OFFSET: f32 = 50.0;

lazy_static! {
    static ref MAIN_MENU: Mutex<Menu> = Mutex::new(Menu::new(
    10.0, PAUSE_MENU_OFFSET,
    vec![
        Box::new(CallbackMenuItem {
            name: "Room Options [soon]",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Inventory",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Player",
            callback: noop,
            on_draw_callback: None,
        }),
        // Box::new(CallbackMenuItem {
        //     name: "Cheats",
        //     callback: noop,
        //     on_draw_callback: None,
        // }),
        Box::new(CallbackMenuItem {
            name: "Warp",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Reload [soon]",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Save [soon]",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Layers [soon]",
            callback: noop,
            on_draw_callback: None,
        }),
        Box::new(CallbackMenuItem {
            name: "Config",
            callback: noop,
            on_draw_callback: None,
        }),
    ]));
}

pub fn draw() {
    let mem = allocated_bytes();
    let max = max_allocated_bytes();
    draw_text(&format!("dyn {}/max {}", mem, max), 10.0, 452.0);

    let mut menu = MAIN_MENU.lock();
    menu.active = true;
    menu.draw();
}

pub fn handle_input() {
    if is_pause_screen() {
        let mut menu = MAIN_MENU.lock();
        menu.handle_input();
    }
}

fn noop(_: &dyn MenuItem) -> OnSelectResult {
    Deslelect
}
