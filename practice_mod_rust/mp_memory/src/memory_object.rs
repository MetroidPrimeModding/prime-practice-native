use alloc::string::String;
use core::ops::{Add, AddAssign};

#[derive(Debug, Copy, Clone)]
pub struct MemoryOffset(pub u32);

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
        (0x8000_0000u32..0x9800_0000u32).contains(&self.0)
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
    fn offset(&self) -> crate::MemoryOffset;
}

#[macro_export]
macro_rules! memory_object {
    ($name: ident) => {
        pub struct $name {
            pub memory: alloc::rc::Rc<dyn crate::MemoryView>,
            pub offset: crate::MemoryOffset,
        }
        impl crate::game_types::Constructable for $name {
            fn new(
                memory: &alloc::rc::Rc<dyn crate::MemoryView>,
                offset: crate::MemoryOffset,
            ) -> Self {
                $name {
                    memory: alloc::rc::Rc::clone(memory),
                    offset: offset,
                }
            }
        }
    };
}

#[macro_export]
macro_rules! memory_field {
  ($name: ident : $type: ty; @ $offset: expr) => {
    pub fn $name(&self) -> Option<$type> {
      Some(<$type>::new(&self.memory, self.offset + $offset))
    }
  };
  (primitive $name: ident : $type: ident @ $offset: expr) => {
    pub fn $name(&self) -> Option<$type> {
      self.memory.$type(self.offset)
    }
  };
  (array $name: ident : $type: ty; @ $offset: expr; size $size: expr) => {
    pub fn $name(&self) -> Option<crate::game_types::Array<$type>> {
      Some($crate::game_types::Array {
        memory: alloc::rc::Rc::clone(&self.memory),
        offset: self.offset + $offset,
        length: $size,
        phantom: core::marker::PhantomData,
      })
    }
  };
}

#[macro_export]
macro_rules! memory_super {
    ($name:ident) => {
        memory_field!(sup: $name; @ 0x0);
    };
}

#[macro_export]
macro_rules! sized_constructable {
    ($name: ident size $size: expr) => {
        impl crate::game_types::SizedConstructable for $name {
            fn size() -> u32 {
                $size
            }
        }
    };
}

macro_rules! primitive {
    ($name: ident : $type: ident size $size: expr) => {
        memory_object!($name);
        impl $name {
          memory_field!(primitive value: $type @ 0x0);
        }
        sized_constructable!($name size $size);
    };
}

pub mod game_types {
    use super::{MemoryOffset, MemoryView};
    use alloc;
    use alloc::rc::Rc;
    use core::marker::PhantomData;

    // #[derive(Copy, Clone)]
    // pub struct MemoryObject {
    //   pub offset: MemoryOffset
    // }

    pub trait Constructable {
        fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self;
    }

    pub trait SizedConstructable: Constructable {
        fn size() -> u32;
    }

    pub struct Array<T: SizedConstructable> {
        pub memory: Rc<dyn MemoryView>,
        pub offset: MemoryOffset,
        pub length: u32,
        pub phantom: PhantomData<T>,
    }

    impl<T: SizedConstructable> Array<T> {
        pub fn size(&self) -> u32 {
            self.length * T::size()
        }

        pub fn get(&self, index: u32) -> Option<T> {
            if index > self.length {
                return None;
            }
            let offset = self.offset + T::size() * index;
            offset.if_valid()?;
            Some(T::new(&self.memory, offset))
        }
    }

    pub struct Enum<T> {
        pub offset: MemoryOffset,
        pub phantom: PhantomData<T>,
    }

    impl<T: From<i32>> Enum<T> {
        pub fn value(&self, view: &dyn MemoryView) -> Option<T> {
            let value = view.i32(self.offset)?;
            Some(T::from(value))
        }
    }

    pub struct Pointer<T: Constructable> {
        pub memory: Rc<dyn MemoryView>,
        pub offset: MemoryOffset,
        pub phantom: PhantomData<T>,
    }

    impl<T: Constructable> Constructable for Pointer<T> {
        fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
            return Pointer {
                memory: Rc::clone(memory),
                offset,
                phantom: PhantomData,
            };
        }
    }

    impl<T: Constructable> SizedConstructable for Pointer<T> {
        fn size() -> u32 {
            4
        }
    }

    impl<T: Constructable> Pointer<T> {
        pub fn value(&self) -> Option<MemoryOffset> {
            let off = self.offset.if_valid()?;
            Some(MemoryOffset(off))
        }

        pub fn deref(&self) -> Option<T> {
            self.offset.if_valid()?;
            Some(T::new(&self.memory, self.offset))
        }
    }

    primitive!(Uint8: u8 size 1);
    primitive!(Int8: i8 size 1);
    primitive!(Uint16: u16 size 2);
    primitive!(Int16: i16 size 2);
    primitive!(Uint32: u32 size 4);
    primitive!(Int32: i32 size 4);
    primitive!(Uint64: u64 size 8);
    primitive!(Int64: i64 size 8);
    primitive!(Uint128: u128 size 16);
    primitive!(Int128: i128 size 16);
    primitive!(Float32: f32 size 4);
    primitive!(Float64: f64 size 8);

    // Custom impl because of weird return type
    memory_object!(String);
    impl String {
        pub fn value(&self) -> Option<alloc::string::String> {
            self.memory.string(self.offset)
        }
    }

    // extra field, so have to implement without the macro :(
    pub struct StringFixedLength {
        pub memory: alloc::rc::Rc<dyn MemoryView>,
        pub offset: MemoryOffset,
        pub length: u32,
    }

    impl StringFixedLength {
        pub fn value(&self) -> Option<alloc::string::String> {
            self.memory.string_fixed_length(self.offset, self.length)
        }
    }
}
