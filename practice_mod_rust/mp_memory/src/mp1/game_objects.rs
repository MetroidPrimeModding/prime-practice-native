use crate::game_types::Constructable;
use crate::game_types::{Float64, Pointer, Uint32};
use crate::mp1::entities::CPlayer;
use crate::MemoryOffset;

memory_object!(CStateManager);
impl CStateManager {
    pub const LOCATION: MemoryOffset = MemoryOffset(0x8045A1A8);

    // memory_field!(allObjects: CObjectList deref 0x810);
    memory_field!(player: Pointer<CPlayer>; @ 0x84C);
    memory_field!(world: Pointer<CWorld>; @ 0x850);
    // memory_field!(cameraManager: CCameraManager deref 0x870);
    // memory_field!(playerState: CPlayerState deref_twice 0x8B8);
    memory_field!(nextAreaID: Uint32; @ 0x8CC);
    memory_field!(prevAreaID: Uint32; @ 0x8D0);
    // memory_field!(random: CRandom16; @ 0x8FC);
}

memory_object!(CSimplePool);

memory_object!(CGameState);
impl CGameState {
    memory_field!(mlvlId: Uint32; @ 0x84);
    memory_field!(playtime: Float64; @ 0xA0);
}

memory_object!(CGameGlobalObjects);
impl CGameGlobalObjects {
    pub const LOCATION: MemoryOffset = MemoryOffset(0x80457798);

    memory_field!(mainPool: CSimplePool; @ 0xCC);
    memory_field!(gameState: Pointer<CGameState>; @ 0x134);
}

memory_object!(CWorld);
impl CWorld {
    memory_field!(phase: Uint32; @ 0x4);
    memory_field!(mlvlID: Uint32; @ 0x8);
    memory_field!(strgID: Uint32; @ 0xC);
    // ...not sure how i'm gonna do this one yet... maybe by hand
    //memory_field!(areas: RSTLVector<RSTLAutoPtr<CGameArea>> @ 0x18)
    memory_field!(relays: Uint32; @ 0x2C);
    memory_field!(currentAreaID: Uint32; @ 0x68);
}
