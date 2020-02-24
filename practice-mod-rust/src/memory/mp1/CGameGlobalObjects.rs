use crate::memory::memory_object::{MemoryOffset, MemoryView};
use crate::memory::mp1::CGameState::CGameState;
use crate::memory::mp1::CSimplePool::CSimplePool;

#[derive(Copy, Clone)]
pub struct CGameGlobalObjects {
  pub offset: MemoryOffset
}

impl CGameGlobalObjects {
  pub fn mainPool(&self) -> Option<CSimplePool> {
    Some(CSimplePool { offset: self.offset + 0xCC })
  }

  pub fn gameState(&self, view: &dyn MemoryView) -> Option<CGameState> {
    let ptr = view.u32(self.offset + 0x134)?;
    Some(CGameState { offset: MemoryOffset(ptr) })
  }
}
