use crate::memory::memory_object::{MemoryOffset, MemoryView};
use crate::memory::mp1::CGameState::CGameState;
use crate::memory::mp1::CSimplePool::CSimplePool;

memory_object!(CGameGlobalObjects);
impl CGameGlobalObjects {
  memory_field!(CSimplePool : mainPool @ 0xCC);
  memory_field!(CGameState : gameState deref 0x134);
}
