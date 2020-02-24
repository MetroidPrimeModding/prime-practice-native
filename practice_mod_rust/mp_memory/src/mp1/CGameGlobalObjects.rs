use crate::mp1::CGameState::CGameState;
use crate::mp1::CSimplePool::CSimplePool;

memory_object!(CGameGlobalObjects);
impl CGameGlobalObjects {
    memory_field!(mainPool : CSimplePool @ 0xCC);
    memory_field!(gameState : CGameState deref 0x134);
}
