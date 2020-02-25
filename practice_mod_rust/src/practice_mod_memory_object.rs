use alloc::string::String;
use mp_memory::{MemoryOffset, MemoryView};

#[derive(Copy, Clone)]
pub struct RealMemoryView;

impl MemoryView for RealMemoryView {
    fn u8(&self, offset: MemoryOffset) -> Option<u8> {
        let ptr = offset.if_valid()? as *const u8;
        unsafe { Some(*ptr) }
    }

    fn i8(&self, offset: MemoryOffset) -> Option<i8> {
        let ptr = offset.if_valid()? as *const i8;
        unsafe { Some(*ptr) }
    }

    fn u16(&self, offset: MemoryOffset) -> Option<u16> {
        let ptr = offset.if_valid()? as *const u16;
        unsafe { Some(*ptr) }
    }

    fn i16(&self, offset: MemoryOffset) -> Option<i16> {
        let ptr = offset.if_valid()? as *const i16;
        unsafe { Some(*ptr) }
    }

    fn u32(&self, offset: MemoryOffset) -> Option<u32> {
        let ptr = offset.if_valid()? as *const u32;
        unsafe { Some(*ptr) }
    }

    fn i32(&self, offset: MemoryOffset) -> Option<i32> {
        let ptr = offset.if_valid()? as *const i32;
        unsafe { Some(*ptr) }
    }

    fn u64(&self, offset: MemoryOffset) -> Option<u64> {
        let ptr = offset.if_valid()? as *const u64;
        unsafe { Some(*ptr) }
    }

    fn i64(&self, offset: MemoryOffset) -> Option<i64> {
        let ptr = offset.if_valid()? as *const i64;
        unsafe { Some(*ptr) }
    }

    fn u128(&self, offset: MemoryOffset) -> Option<u128> {
        let ptr = offset.if_valid()? as *const u128;
        unsafe { Some(*ptr) }
    }

    fn i128(&self, offset: MemoryOffset) -> Option<i128> {
        let ptr = offset.if_valid()? as *const i128;
        unsafe { Some(*ptr) }
    }

    fn f32(&self, offset: MemoryOffset) -> Option<f32> {
        let ptr = offset.if_valid()? as *const f32;
        unsafe { Some(*ptr) }
    }

    fn f64(&self, offset: MemoryOffset) -> Option<f64> {
        let ptr = offset.if_valid()? as *const f64;
        unsafe { Some(*ptr) }
    }

    fn string_fixed_length(&self, offset: MemoryOffset, size: u32) -> Option<String> {
        let start: u32 = offset.into();
        let end: u32 = (offset + size).into();
        let mut res = String::with_capacity(size as usize);
        for i in (start)..(end) {
            res.push(self.u8(i.into())? as char);
        }
        Some(res)
    }

    fn string(&self, offset: MemoryOffset) -> Option<String> {
        let mut res = String::new();
        let mut i = offset;
        loop {
            let char = self.u8(i)?;
            if char == 0 {
                break;
            }
            res.push(char as char);
            i += 1;
        }
        Some(res)
    }
}
