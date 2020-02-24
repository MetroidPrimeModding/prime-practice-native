#![no_std]
#![feature(alloc_error_handler)]

extern crate alloc;

use core::alloc::{GlobalAlloc, Layout};
use core::ffi::c_void;
use core::fmt::{self, Write};

// Set up calls into our other code, courtesy the randomprime repo
extern "C" {
  pub fn printf(fmt: *const u8, ...);

  pub fn sprintf(s: *mut u8, fmt: *const u8, ...);

  fn malloc(len: u32) -> *mut c_void;
  fn free(ptr: *const c_void);
}

struct MPAlloc;

unsafe impl GlobalAlloc for MPAlloc {
  unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
    malloc(layout.size() as u32) as *mut u8
  }
  unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout)
  {
    free(ptr as *const c_void)
  }
}

#[global_allocator]
static ALLOC: MPAlloc = MPAlloc;

pub struct MPStdout;

impl fmt::Write for MPStdout {
  fn write_str(&mut self, s: &str) -> fmt::Result {
    unsafe {
      printf("%s".as_bytes().as_ptr(), s.as_bytes().as_ptr());
    }
    Ok(())
  }
}

fn halt() -> ! {
  // extern "C" {
  //     fn PPCHalt() -> !;
  // }
  // unsafe {
  //     PPCHalt()
  // }
  loop {}
}

#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
  if cfg!(debug_assertions) {
    writeln!(MPStdout, "{}", info).ok();
  }

  halt()
}

#[alloc_error_handler]
fn alloc_error(_layout: Layout) -> ! {
  if cfg!(debug_assertions) {
    writeln!(MPStdout, "Alloc failed").ok();
  }

  halt()
}

// Library methods
#[no_mangle]
pub extern "C" fn hello() -> *const u8 {
  return &"Hello, Rust!".as_bytes()[0];
}

pub mod hooks;
pub use hooks::on_frame;

pub mod memory {
  #[macro_use]
  pub mod memory_object;
  #[allow(non_snake_case)]
  pub mod mp1 {
    pub mod CGameState;
    pub mod CGameGlobalObjects;
    pub mod CSimplePool;
  }
}

pub mod practice_mod_memory_object;
