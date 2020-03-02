use crate::hooks::pause_screen::view::HandleInputResult::{DoNothing, Pop};
use alloc::boxed::Box;
use alloc::vec::Vec;

pub enum HandleInputResult {
    DoNothing,
    Pop,
    Push(Box<dyn View>),
}

pub trait View: Send {
    fn draw(&self);
    fn handle_input(&mut self) -> HandleInputResult;
}

pub struct NavController {
    stack: Vec<Box<dyn View>>,
}

impl NavController {
    pub fn new(root: Box<dyn View>) -> Self {
        NavController { stack: vec![root] }
    }

    pub fn push(&mut self, view: Box<dyn View>) {
        self.stack.push(view);
    }

    pub fn pop(&mut self) {
        self.stack.pop();
    }

    pub fn replace_top(&mut self, view: Box<dyn View>) {
        self.pop();
        self.push(view);
    }

    pub fn is_empty(&self) -> bool {
        self.stack.is_empty()
    }
}

impl View for NavController {
    fn draw(&self) {
        if let Some(v) = self.stack.last() {
            v.draw();
        }
    }

    fn handle_input(&mut self) -> HandleInputResult {
        if let Some(v) = self.stack.last_mut() {
            let res = v.handle_input();
            match res {
                DoNothing => {}
                HandleInputResult::Pop => self.pop(),
                HandleInputResult::Push(v) => self.push(v),
            }
        }

        if self.is_empty() {
            Pop
        } else {
            DoNothing
        }
    }
}
