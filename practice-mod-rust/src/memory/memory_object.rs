use alloc::string::String;
use core::ops::{Add, AddAssign};

#[derive(Debug, Copy, Clone)]
pub struct MemoryOffset(pub u32);

impl Add<u32> for MemoryOffset {
  type Output = MemoryOffset;

  fn add(self, rhs: u32) -> Self::Output {
    MemoryOffset(self.0 + rhs)
  }
}

impl AddAssign<u32> for MemoryOffset {
  fn add_assign(&mut self, rhs: u32) {
    self.0 = self.0 + rhs;
  }
}

impl MemoryOffset {
  pub fn is_valid(&self) -> bool {
    (0x8000000u32..0x98000000u32).contains(&self.0)
  }

  pub fn if_valid(&self) -> Option<u32> {
    if self.is_valid() {
      Some(self.0)
    } else {
      None
    }
  }
}

pub trait MemoryView {
  fn u8(&self, offset: MemoryOffset) -> Option<u8>;
  fn i8(&self, offset: MemoryOffset) -> Option<i8>;

  fn u16(&self, offset: MemoryOffset) -> Option<u16>;
  fn i16(&self, offset: MemoryOffset) -> Option<i16>;

  fn u32(&self, offset: MemoryOffset) -> Option<u32>;
  fn i32(&self, offset: MemoryOffset) -> Option<i32>;

  fn u64(&self, offset: MemoryOffset) -> Option<u64>;
  fn i64(&self, offset: MemoryOffset) -> Option<i64>;

  fn u128(&self, offset: MemoryOffset) -> Option<u128>;
  fn i128(&self, offset: MemoryOffset) -> Option<i128>;

  fn f32(&self, offset: MemoryOffset) -> Option<f32>;
  fn f64(&self, offset: MemoryOffset) -> Option<f64>;

  fn string_fixed_length(&self, offset: MemoryOffset, size: u32) -> Option<String>;
  fn string(&self, offset: MemoryOffset) -> Option<String>;
}


pub mod game_types {
  use super::{MemoryOffset, MemoryView};
  use alloc;
  use core::marker::PhantomData;

  #[derive(Clone)]
  pub struct Array<T, F: Fn(&dyn MemoryView, MemoryOffset) -> T> {
    pub offset: MemoryOffset,
    pub stride: u32,
    pub length: u32,
    construct: F,
  }

  impl<T, F: Fn(&dyn MemoryView, MemoryOffset) -> T> Array<T, F> {
    pub fn get(&self, view: &dyn MemoryView, index: u32) -> Option<T> {
      let offset = self.offset + self.stride * index;
      offset.if_valid()?;
      Some((self.construct)(view, offset))
    }
  }

  #[derive(Copy, Clone)]
  pub struct Enum<T> {
    pub offset: MemoryOffset,
    phantom: PhantomData<T>,
  }

  impl<T: From<i32>> Enum<T> {
    pub fn value(&self, view: &dyn MemoryView) -> Option<T> {
      let value = view.i32(self.offset)?;
      Some(T::from(value))
    }
  }

  #[derive(Copy, Clone)]
  pub struct Uint8 {
    pub offset: MemoryOffset
  }

  impl Uint8 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<u8> {
      view.u8(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Int8 {
    pub offset: MemoryOffset
  }

  impl Int8 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<i8> {
      view.i8(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Uint16 {
    pub offset: MemoryOffset
  }

  impl Uint16 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<u16> {
      view.u16(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Int16 {
    pub offset: MemoryOffset
  }

  impl Int16 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<i16> {
      view.i16(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Uint32 {
    pub offset: MemoryOffset
  }

  impl Uint32 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<u32> {
      view.u32(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Int32 {
    pub offset: MemoryOffset
  }

  impl Int32 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<i32> {
      view.i32(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Uint64 {
    pub offset: MemoryOffset
  }

  impl Uint64 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<u64> {
      view.u64(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Int64 {
    pub offset: MemoryOffset
  }

  impl Int64 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<i64> {
      view.i64(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Float32 {
    pub offset: MemoryOffset
  }

  impl Float32 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<f32> {
      view.f32(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct Float64 {
    pub offset: MemoryOffset
  }

  impl Float64 {
    pub fn value(&self, view: &dyn MemoryView) -> Option<f64> {
      view.f64(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct String {
    pub offset: MemoryOffset
  }

  impl String {
    pub fn value(&self, view: &dyn MemoryView) -> Option<alloc::string::String> {
      view.string(self.offset)
    }
  }

  #[derive(Copy, Clone)]
  pub struct StringFixedLength {
    pub offset: MemoryOffset,
    pub length: u32
  }

  impl StringFixedLength {
    pub fn value(&self, view: &dyn MemoryView) -> Option<alloc::string::String> {
      view.string_fixed_length(self.offset, self.length)
    }
  }
}


