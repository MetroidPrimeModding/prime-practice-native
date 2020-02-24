#![no_std]

extern crate alloc;

#[macro_use]
pub mod memory_object;

#[allow(non_snake_case)]
pub mod mp1 {
    pub mod CGameGlobalObjects;
    pub mod CGameState;
    pub mod CSimplePool;
}
