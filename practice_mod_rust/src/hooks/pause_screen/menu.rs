use crate::cpp_interface::text_renderer::{
    draw_text, input_down, input_down_fast, input_pressed_back, input_pressed_ok, input_up,
    input_up_fast, set_text_color,
};
use crate::hooks::pause_screen::view::HandleInputResult::{DoNothing, Pop};
use crate::hooks::pause_screen::view::{HandleInputResult, View};
use crate::hooks::pause_screen::PAUSE_MENU_OFFSET;
use crate::hooks::LINE_HEIGHT;
use alloc::boxed::Box;
use alloc::vec::Vec;

pub struct MenuItem<'a> {
    pub name: &'a str,
    pub on_select: Box<dyn Fn() -> HandleInputResult + Send>,
}

pub struct Menu<'a> {
    items: Vec<MenuItem<'a>>,
    cursor: usize,
    scroll_timer: u32,
    pub can_pop: bool,
}

impl<'a> Menu<'a> {
    pub fn new(items: Vec<MenuItem<'a>>) -> Self {
        Menu {
            items,
            cursor: 0,
            scroll_timer: 0,
            can_pop: true,
        }
    }

    pub fn select(&mut self) -> HandleInputResult {
        (self.items[self.cursor].on_select)()
    }

    pub fn modify_cursor(&mut self, amount: i32, timer: u32) {
        if amount >= 0 {
            self.move_cursor_down(amount as usize, timer)
        } else {
            self.move_cursor_up((-amount) as usize, timer)
        }
    }

    pub fn move_cursor_down(&mut self, amount: usize, timer: u32) {
        if self.scroll_timer == 0 {
            self.scroll_timer = timer;
            self.cursor = (self.cursor + amount as usize) % self.items.len();
        }
    }

    pub fn move_cursor_up(&mut self, amount: usize, timer: u32) {
        if self.scroll_timer == 0 {
            self.scroll_timer = timer;
            let real_amt = amount % self.items.len();
            if real_amt > self.cursor {
                self.cursor = self.cursor + self.items.len() - real_amt
            } else {
                self.cursor = self.cursor - real_amt;
            }
        }
    }
}

impl<'a> View for Menu<'a> {
    fn draw(&self) {
        let mut y_off = 0f32;
        let scroll_off = 30usize;
        if self.cursor > scroll_off {
            y_off = (self.cursor - scroll_off) as f32 * -LINE_HEIGHT
        }
        for (i, child) in self.items.iter().enumerate() {
            let y = i as f32 * LINE_HEIGHT + y_off;
            if y < 0.0 || y > 390.0 {
                continue;
            }
            if self.cursor == i {
                set_text_color(1.0, 1.0, 1.0, 1.0);
            } else {
                set_text_color(0.4, 0.4, 0.4, 1.0);
            }
            draw_text(child.name, 10.0, PAUSE_MENU_OFFSET + y)
        }
    }

    fn handle_input(&mut self) -> HandleInputResult {
        if self.scroll_timer > 0 {
            self.scroll_timer -= 1;
        }
        if input_down() {
            self.modify_cursor(1, 8);
        }
        if input_up() {
            self.modify_cursor(-1, 8);
        }
        if input_down_fast() {
            self.modify_cursor(10, 15);
        }
        if input_up_fast() {
            self.modify_cursor(-10, 15);
        }
        if input_pressed_ok() {
            return self.select();
        }
        if self.can_pop && input_pressed_back() {
            return Pop;
        }
        return DoNothing;
    }
}
