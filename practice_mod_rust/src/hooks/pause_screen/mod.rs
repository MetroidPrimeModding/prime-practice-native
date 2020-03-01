use crate::cpp_interface::text_renderer::{draw_text, is_pause_screen};
use crate::hooks::pause_screen::OnSelectResult::{Deslelect, DoNothing};
use crate::{allocated_bytes, max_allocated_bytes, MPStdout};
use alloc::boxed::Box;
use core::fmt::Write;
pub use menu::*;
use spin::Mutex;
use warps::{Warp, WarpWorldItem, CHOZO, CRATER, FRIGATE, MAGMOOR, MINES, PHENDRANA, TALLON};

mod menu;
mod warps;

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
        Box::new(SubmenuMenuItem {
            name: "Warp",
            submenu: Menu::new(
                MENU_2_OFFSET_X, PAUSE_MENU_OFFSET, vec![
                    Box::new(WarpWorldItem::new(
                        "Frigate Orpheon",
                        0x158EFE17,
                        &FRIGATE
                    )),
                    Box::new(WarpWorldItem::new(
                        "Tallon Overworld",
                        0x39F2DE28,
                        &TALLON
                    )),
                    Box::new(WarpWorldItem::new(
                        "Chozo Ruins",
                        0x83F6FF6F,
                        &CHOZO
                    )),
                    Box::new(WarpWorldItem::new(
                        "Magmoor Caverns",
                        0x3EF8237C,
                        &MAGMOOR
                    )),
                    Box::new(WarpWorldItem::new(
                        "Phendrana Drifts",
                        0xA8BE6291,
                        &PHENDRANA
                    )),
                    Box::new(WarpWorldItem::new(
                        "Phazon Mines",
                        0xB1AC4D65,
                        &MINES
                    )),
                    Box::new(WarpWorldItem::new(
                        "Impact Crater",
                        0xC13B09D1,
                        &CRATER
                    ))
                ]
            )
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
