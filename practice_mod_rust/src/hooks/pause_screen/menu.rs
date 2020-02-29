use crate::cpp_interface::text_renderer::{draw_text, set_text_color};
use crate::hooks::{CHAR_DIM, LINE_HEIGHT};
use alloc::boxed::Box;
use alloc::rc::Rc;
use alloc::vec::Vec;
use core::mem::Discriminant;
use HandleInputResult::Propagate;
use OnSelectResult::DoNothing;

pub enum OnSelectResult {
    DoNothing,
    Deslelect,
}

pub enum HandleInputResult {
    Propagate,
    StopPropagation,
}

trait MenuItem {
    fn on_select(&self) -> OnSelectResult {
        DoNothing
    }
    fn handle_input(&self) -> HandleInputResult {
        Propagate
    }
    fn draw(&self, x: f32, y: f32);
}

struct Menu {
    pub cursor: usize,
    pub x: f32,
    pub y: f32,
    pub has_selected: bool,
    pub active: bool,
    pub scroll_timer: u32,
    children: Vec<Rc<dyn MenuItem>>,
}

impl Menu {
    fn new(children: Vec<Rc<dyn MenuItem>>) -> Self {
        Menu {
            cursor: 0,
            x: 0.0,
            y: 0.0,
            has_selected: false,
            active: false,
            scroll_timer: 0,
            children,
        }
    }

    fn select(&mut self, index: usize) -> OnSelectResult {
        if index > self.children.len() {
            return DoNothing;
        }
        self.cursor = index;
        self.has_selected = true;
        self.children[index].on_select()
    }

    fn deselect(&mut self) {
        self.has_selected = false;
    }

    fn draw(&self) {
        let mut y_off = 0f32;
        let scroll_off = 30usize;

        if self.cursor > scroll_off {
            y_off = (self.cursor - scroll_off) as f32 * -LINE_HEIGHT
        }
        for (i, child) in self.children.iter().enumerate() {
            let y = i as f32 * LINE_HEIGHT + y_off;
            if y < 0.0 || y > 400.0 {
                continue;
            }
            if self.cursor == i {
                if self.has_selected {
                    set_text_color(0.7, 0.7, 1.0, 1.0);
                } else {
                    set_text_color(1.0, 1.0, 1.0, 1.0);
                }
            } else {
                set_text_color(0.4, 0.4, 0.4, 1.0);
            }
            child.draw(self.x, self.y + y)
        }
    }

    fn handle_input(&self) -> HandleInputResult {
        if self.has_selected {
            return self.children[self.cursor].handle_input();
        }
        Propagate
    }
}

struct CallbackMenuItem<'a> {
    pub name: &'a str,
    pub callback: fn(&dyn MenuItem) -> OnSelectResult,
    pub on_draw_callback: Option<fn(&dyn MenuItem, f32, f32)>,
}

impl<'a> MenuItem for CallbackMenuItem<'a> {
    fn on_select(&self) -> OnSelectResult {
        (self.callback)(self)
    }

    fn draw(&self, x: f32, y: f32) {
        if let Some(cb) = self.on_draw_callback {
            (cb)(self, x, y);
        }
        draw_text(self.name, x, y)
    }
}
