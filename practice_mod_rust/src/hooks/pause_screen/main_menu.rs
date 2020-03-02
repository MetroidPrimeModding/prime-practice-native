use crate::hooks::pause_screen::view::HandleInputResult::{DoNothing, Push};
use crate::hooks::pause_screen::warps::create_warps_menu;
use crate::hooks::pause_screen::{Menu, MenuItem};
use alloc::boxed::Box;

pub fn create_main_menu<'a>() -> Menu<'a> {
    let mut res = Menu::new(vec![
        MenuItem {
            name: "Room Options [Soon]",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Inventory",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Player",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Cheats [Soon]",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Warp",
            on_select: Box::new(|| Push(Box::new(create_warps_menu()))),
        },
        MenuItem {
            name: "Reload [soon]",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Save [soon]",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Layers [soon]",
            on_select: Box::new(|| DoNothing),
        },
        MenuItem {
            name: "Config",
            on_select: Box::new(|| DoNothing),
        },
    ]);
    res.can_pop = false;
    res
}
