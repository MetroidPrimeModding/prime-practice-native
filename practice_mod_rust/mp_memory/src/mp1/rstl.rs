use crate::game_types::{Constructable, SizedConstructable};
use crate::game_types::{Pointer, Uint32, Uint8};
use crate::{game_types, MemoryOffset, MemoryView};
use alloc::rc::Rc;
use core::marker::PhantomData;

//////////
//Vector//
//////////
pub struct Vector<T: SizedConstructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub phantom: PhantomData<T>,
}

impl<T: SizedConstructable> Constructable for Vector<T> {
    fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
        Vector {
            memory: Rc::clone(memory),
            offset,
            phantom: PhantomData,
        }
    }
}

impl<T: SizedConstructable> SizedConstructable for Vector<T> {
    fn size() -> u32 {
        12
    }
}

impl<T: SizedConstructable> Vector<T> {
    memory_field!(end: Uint32; @ 0x0);
    memory_field!(size: Uint32; @ 0x4);
    memory_field!(first: Pointer<T>; @ 0x4);

    pub fn get(&self, index: u32) -> Option<T> {
        if index > self.end()?.value()? {
            return None;
        }
        let offset = self.offset + T::size() * index;
        offset.if_valid()?;
        Some(T::new(&self.memory, offset))
    }
}

////////
//PAIR//
////////
pub struct CPair<T1: SizedConstructable, T2: SizedConstructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub phantom1: PhantomData<T1>,
    pub phantom2: PhantomData<T2>,
}

impl<T1: SizedConstructable, T2: SizedConstructable> Constructable for CPair<T1, T2> {
    fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
        CPair {
            memory: Rc::clone(memory),
            offset,
            phantom1: PhantomData,
            phantom2: PhantomData,
        }
    }
}

impl<T1: SizedConstructable, T2: SizedConstructable> SizedConstructable for CPair<T1, T2> {
    fn size() -> u32 {
        T1::size() + T2::size()
    }
}

impl<T1: SizedConstructable, T2: SizedConstructable> CPair<T1, T2> {
    memory_field!(a: T1; @ 0x0);
    memory_field!(b: T2; @ T1::size());
}

///////////
//CRBTree//
///////////
pub struct CRBTree<T: Constructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub phantom: PhantomData<T>,
}

impl<T: Constructable> Constructable for CRBTree<T> {
    fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
        CRBTree {
            memory: Rc::clone(memory),
            offset,
            phantom: PhantomData,
        }
    }
}

impl<T: Constructable> SizedConstructable for CRBTree<T> {
    fn size() -> u32 {
        16
    }
}

impl<T: SizedConstructable> CRBTree<T> {
    memory_field!(treeSize: Uint32; @ 0x0);
    memory_field!(first: Pointer<CRBTreeNode<T>>; @ 0x4);
    memory_field!(last: Pointer<CRBTreeNode<T>>; @ 0x8);
    memory_field!(root: Pointer<CRBTreeNode<T>>; @ 0xC);
}

///////////////
//CRBTreeNode//
///////////////
pub struct CRBTreeNode<T: Constructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub phantom: PhantomData<T>,
}

impl<T: Constructable> Constructable for CRBTreeNode<T> {
    fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
        CRBTreeNode {
            memory: Rc::clone(memory),
            offset,
            phantom: PhantomData,
        }
    }
}

impl<T: Constructable> SizedConstructable for CRBTreeNode<T> {
    fn size() -> u32 {
        20
    }
}

impl<T: Constructable> CRBTreeNode<T> {
    memory_field!(left: Pointer<CRBTreeNode<T>>; @ 0x0);
    memory_field!(right: Pointer<CRBTreeNode<T>>; @ 0x4);
    memory_field!(parent: Pointer<CRBTreeNode<T>>; @ 0x8);
    memory_field!(redOrBlack: Uint32; @ 0xC);
    memory_field!(data: Pointer<T>; @ 0x10);
}

///////////
//AutoPtr//
///////////
pub struct AutoPtr<T: Constructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub phantom: PhantomData<T>,
}

impl<T: Constructable> Constructable for AutoPtr<T> {
    fn new(memory: &Rc<dyn MemoryView>, offset: MemoryOffset) -> Self {
        AutoPtr {
            memory: Rc::clone(memory),
            offset,
            phantom: PhantomData,
        }
    }
}

impl<T: Constructable> SizedConstructable for AutoPtr<T> {
    fn size() -> u32 {
        8
    }
}

impl<T: Constructable> AutoPtr<T> {
    memory_field!(unknown: Uint32; @ 0x0);
    memory_field!(referenced: Uint8; @ 0x0);
    memory_field!(data: Pointer<T>; @ 0x4);
}

//////////////////
//ReservedVector//
//////////////////
pub struct ReservedVector<T: SizedConstructable> {
    pub memory: Rc<dyn MemoryView>,
    pub offset: MemoryOffset,
    pub capacity: u32,
    pub phantom: PhantomData<T>,
}

impl<T: SizedConstructable> ReservedVector<T> {
    memory_field!(size: Uint32; @ 0x0);
    // Manually implemented because it uses self.capacity
    pub fn array(&self) -> Option<game_types::Array<T>> {
        Some(game_types::Array {
            memory: Rc::clone(&self.memory),
            offset: self.offset + 0x4,
            length: self.capacity,
            phantom: core::marker::PhantomData,
        })
    }

    pub fn get(&self, index: u32) -> Option<T> {
        if index > self.size()?.value()? {
            return None;
        }
        self.array()?.get(index)
    }
}
