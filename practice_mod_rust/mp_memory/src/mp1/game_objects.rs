use crate::game_types::{Float64, Uint32};
use crate::mp1::entities::CPlayer;
use crate::MemoryOffset;

memory_object!(CStateManager);
impl CStateManager {
    pub const LOCATION: MemoryOffset = MemoryOffset(0x8045A1A8);

    // memory_field!(allObjects: CObjectList deref 0x810);
    memory_field!(player: CPlayer deref 0x84C);
    // memory_field!(world: CWorld deref 0x850);
    // memory_field!(cameraManager: CCameraManager deref 0x870);
    // memory_field!(playerState: CPlayerState deref_twice 0x8B8);
    memory_field!(nextAreaID: Uint32 @ 0x8CC);
    memory_field!(prevAreaID: Uint32 @ 0x8D0);
    // memory_field!(random: CRandom16 @ 0x8FC);
}

memory_object!(CSimplePool);

memory_object!(CGameState);
impl CGameState {
    memory_field!(mlvlId: Uint32 @ 0x84);
    memory_field!(playtime: Float64 @ 0xA0);
}

memory_object!(CGameGlobalObjects);
impl CGameGlobalObjects {
    pub const LOCATION: MemoryOffset = MemoryOffset(0x80457798);

    memory_field!(mainPool: CSimplePool @ 0xCC);
    memory_field!(gameState: CGameState deref 0x134);
}
