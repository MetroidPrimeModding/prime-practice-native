use alloc::rc::Rc;

use crate::practice_mod_memory_object::RealMemoryView;
use mp_memory::mp1::CGameGlobalObjects;
use mp_memory::mp1::CStateManager;
use mp_memory::MemoryView;

pub fn real_memory() -> Rc<dyn MemoryView> {
    Rc::new(RealMemoryView)
}

pub fn global_objects() -> CGameGlobalObjects {
    CGameGlobalObjects {
        memory: real_memory(),
        offset: CGameGlobalObjects::LOCATION,
    }
}

pub fn state_manager() -> CStateManager {
    CStateManager {
        memory: real_memory(),
        offset: CGameGlobalObjects::LOCATION,
    }
}
