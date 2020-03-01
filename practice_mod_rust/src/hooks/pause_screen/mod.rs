use crate::cpp_interface::text_renderer::draw_text;
use crate::hooks::pause_screen::OnSelectResult::DoNothing;
use crate::{allocated_bytes, max_allocated_bytes};
use alloc::boxed::Box;
pub use menu::*;
use spin::Mutex;

mod menu;

lazy_static! {
    static ref MAIN_MENU: Mutex<Menu> = Mutex::new(Menu::new(vec![
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

    // unsafe {
    let mut menu = MAIN_MENU.lock();
    menu.active = true;
    menu.draw();
    // }
}

fn noop(_: &dyn MenuItem) -> OnSelectResult {
    DoNothing
}
