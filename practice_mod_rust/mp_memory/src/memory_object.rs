use alloc::string::String;
use core::ops::{Add, AddAssign};

#[derive(Debug, Copy, Clone)]
pub struct MemoryOffset(u32);

impl MemoryOffset {
    fn of(v: u32) -> Self {
        return MemoryOffset(v);
    }
}

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

impl From<u32> for MemoryOffset {
    fn from(v: u32) -> Self {
        MemoryOffset::of(v)
    }
}

impl From<MemoryOffset> for u32 {
    fn from(v: MemoryOffset) -> Self {
        v.0
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

trait MemoryObject {
    fn offset(&self) -> crate::memory_object::MemoryOffset;
}

#[macro_export]
macro_rules! memory_object {
    ($name: ident) => {
        pub struct $name {
            pub memory: alloc::rc::Rc<dyn crate::memory_object::MemoryView>,
            pub offset: crate::memory_object::MemoryOffset,
        }
    };
}

#[macro_export]
macro_rules! memory_field {
  ($name: ident : $type: ident @ $offset: expr) => {
    pub fn $name(&self) -> Option<$type> {
      Some($type { offset: self.offset + $offset })
    }
  };
  ($name: ident : $type: ident deref $offset: expr) => {
    pub fn $name(&self) -> Option<$type> {
      let ptr = self.memory.u32(self.offset + $offset)?;
      Some($type { offset: ptr.into() })
    }
  };
  ($name: ident : $type: ident by primitive) => {
    pub fn $name(&self) -> Option<$type> {
      self.memory.$type(self.offset)
    }
  }
}

macro_rules! primitive {
  ($name: ident : $type: ident) => {
    memory_object!($name);
    impl $name {
      memory_field!(value: $type by primitive);
    }
  }
}

pub mod game_types {
    use super::{MemoryOffset, MemoryView};
    use alloc;
    use core::marker::PhantomData;

    // #[derive(Copy, Clone)]
    // pub struct MemoryObject {
    //   pub offset: MemoryOffset
    // }

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

    primitive!(Uint8: u8);
    primitive!(Int8: i8);
    primitive!(Uint16: u16);
    primitive!(Int16: i16);
    primitive!(Uint32: u32);
    primitive!(Int32: i32);
    primitive!(Uint64: u64);
    primitive!(Int64: i64);
    primitive!(Uint128: u128);
    primitive!(Int128: i128);
    primitive!(Float32: f32);
    primitive!(Float64: f64);
    // primitive!(String: string);

    // memory_object!(StringFixedLength, length: u32);

    // #[derive(Copy, Clone)]
    // pub struct StringFixedLength {
    //   pub offset: MemoryOffset,
    //   pub length: u32
    // }
    //
    // impl StringFixedLength {
    //   pub fn value(&self, view: &dyn MemoryView) -> Option<alloc::string::String> {
    //     view.string_fixed_length(self.offset, self.length)
    //   }
    // }
}
