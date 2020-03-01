#![no_std]
#![feature(alloc_error_handler)]
#![feature(lang_items)]

#[macro_use]
extern crate alloc;

#[macro_use]
extern crate lazy_static;

extern crate spin;

use core::alloc::{GlobalAlloc, Layout};
use core::ffi::c_void;
use core::fmt::{self, Write};

// Set up calls into our other code, courtesy the randomprime repo
extern "C" {
    fn rust_error(fmt: *const u8, len: u32);

    fn malloc(len: u32) -> *mut c_void;
    fn free(ptr: *const c_void);
}

struct MPAlloc;

static mut ALLOC_BYTES: u32 = 0;
static mut MAX_ALLOC_BYTES: u32 = 0;

unsafe impl GlobalAlloc for MPAlloc {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        ALLOC_BYTES += layout.size() as u32;
        if ALLOC_BYTES > MAX_ALLOC_BYTES {
            MAX_ALLOC_BYTES = ALLOC_BYTES;
        }
        malloc(layout.size() as u32) as *mut u8
    }
    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
        ALLOC_BYTES -= layout.size() as u32;
        free(ptr as *const c_void)
    }
}

pub fn allocated_bytes() -> u32 {
    unsafe { ALLOC_BYTES }
}

pub fn max_allocated_bytes() -> u32 {
    unsafe { MAX_ALLOC_BYTES }
}

#[global_allocator]
static ALLOC: MPAlloc = MPAlloc;

pub struct MPStdout;

impl fmt::Write for MPStdout {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        unsafe {
            rust_error(s.as_ptr(), s.len() as u32);
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
    writeln!(MPStdout, "{}\n", info).ok();
    halt()
}

#[alloc_error_handler]
fn alloc_error(_layout: Layout) -> ! {
    writeln!(MPStdout, "Alloc failed\n").ok();
    halt()
}

// Library methods
#[no_mangle]
pub extern "C" fn hello() -> *const u8 {
    return &"Hello, Rust!".as_bytes()[0];
}

pub mod hooks;

pub use hooks::on_frame;

pub mod cpp_interface;
pub mod globals;
pub mod practice_mod_memory_object;
