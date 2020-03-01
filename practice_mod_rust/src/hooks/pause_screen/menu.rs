use crate::cpp_interface::text_renderer::{
    draw_text, input_down, input_down_fast, input_pressed_back, input_pressed_ok, input_up,
    input_up_fast, set_text_color,
};
use crate::hooks::pause_screen::HandleInputResult::{Deselect, StopPropagation};
use crate::hooks::pause_screen::OnSelectResult::Deslelect;
use crate::hooks::LINE_HEIGHT;
use alloc::boxed::Box;
use alloc::vec::Vec;
use HandleInputResult::Propagate;
use OnSelectResult::DoNothing;

pub enum OnSelectResult {
    DoNothing,
    Deslelect,
}

pub enum HandleInputResult {
    Deselect,
    Propagate,
    StopPropagation,
}

pub trait MenuItem: Sync + Send {
    fn on_select(&mut self) -> OnSelectResult {
        Deslelect
    }
    fn handle_input(&mut self) -> HandleInputResult {
        Propagate
    }
    fn draw(&self, x: f32, y: f32);
}

pub struct Menu {
    pub cursor: i32,
    pub x: f32,
    pub y: f32,
    pub has_selected: bool,
    pub active: bool,
    pub scroll_timer: i32,
    children: Vec<Box<dyn MenuItem>>,
}

impl Menu {
    pub fn new(x: f32, y: f32, children: Vec<Box<dyn MenuItem>>) -> Self {
        Menu {
            cursor: 0,
            x,
            y,
            has_selected: false,
            active: false,
            scroll_timer: 0,
            children,
        }
    }

    pub fn select(&mut self, index: usize) -> OnSelectResult {
        if index > self.children.len() {
            return DoNothing;
        }
        self.cursor = index as i32;
        self.has_selected = true;
        self.children[index].on_select()
    }

    pub fn deselect(&mut self) {
        self.has_selected = false;
    }

    pub fn draw(&self) {
        if !self.active {
            return;
        }
        let mut y_off = 0f32;
        let scroll_off = 30i32;

        if self.cursor > scroll_off {
            y_off = (self.cursor - scroll_off) as f32 * -LINE_HEIGHT
        }
        for (i, child) in self.children.iter().enumerate() {
            let y = i as f32 * LINE_HEIGHT + y_off;
            if y < 0.0 || y > 400.0 {
                continue;
            }
            if self.cursor == i as i32 {
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

    pub fn handle_input(&mut self) -> HandleInputResult {
        if self.has_selected {
            let res = self.children[self.cursor as usize].handle_input();
            if let Deselect = res {
                self.deselect();
                return StopPropagation;
            }
            if let StopPropagation = res {
                return StopPropagation;
            }
        }
        if self.scroll_timer > 0 {
            self.scroll_timer -= 1;
        }
        if input_down() {
            if self.scroll_timer <= 0 {
                self.cursor += 1;
                self.scroll_timer = 8;
            }
        }
        if input_up() {
            if self.scroll_timer <= 0 {
                self.cursor -= 1;
                self.scroll_timer = 8;
            }
        }
        if input_down_fast() {
            if self.scroll_timer <= 0 {
                self.cursor += 10;
                self.scroll_timer = 15;
            }
        }
        if input_up_fast() {
            if self.scroll_timer <= 0 {
                self.cursor -= 10;
                self.scroll_timer = 15;
            }
        }
        if input_pressed_ok() {
            let result = self.select(self.cursor as usize);
            match result {
                DoNothing => {}
                OnSelectResult::Deslelect => self.deselect(),
            };
        }

        self.cursor = self.cursor % self.children.len() as i32;
        if self.cursor < 0 {
            self.cursor += self.children.len() as i32;
        }
        Propagate
    }
}

pub struct CallbackMenuItem<'a> {
    pub name: &'a str,
    pub callback: fn(&dyn MenuItem) -> OnSelectResult,
    pub on_draw_callback: Option<fn(&dyn MenuItem, f32, f32)>,
}

impl<'a> MenuItem for CallbackMenuItem<'a> {
    fn on_select(&mut self) -> OnSelectResult {
        (self.callback)(self)
    }

    fn draw(&self, x: f32, y: f32) {
        if let Some(cb) = self.on_draw_callback {
            (cb)(self, x, y);
        }
        draw_text(self.name, x, y)
    }
}

pub struct SubmenuMenuItem<'a> {
    pub name: &'a str,
    pub submenu: Menu,
}

impl<'a> MenuItem for SubmenuMenuItem<'a> {
    fn on_select(&mut self) -> OnSelectResult {
        self.submenu.active = true;
        self.submenu.deselect();
        DoNothing
    }

    fn handle_input(&mut self) -> HandleInputResult {
        self.submenu.handle_input();

        if input_pressed_back() {
            self.submenu.active = false;
            self.submenu.deselect();
            return Deselect;
        }

        StopPropagation
    }

    fn draw(&self, x: f32, y: f32) {
        draw_text(self.name, x, y);
        self.submenu.draw();
    }
}
