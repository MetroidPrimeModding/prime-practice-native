use crate::game_types::Constructable;
use crate::game_types::{String, Uint32};
use crate::mp1::math::{CAABBPrimitive, CQuaternion, CTransform, CVector3f};

memory_object!(CEntity);
impl CEntity {
    memory_field!(vtable: Uint32; @ 0x0);
    memory_field!(areaID: Uint32; @ 0x4);
    memory_field!(uniqueID: Uint32; @ 0x8);
    memory_field!(editorID: Uint32; @ 0xC);
    memory_field!(name: String; @ 0x10);
    memory_field!(status: Uint32; @ 0x30);
}

memory_object!(CActor);
impl CActor {
    memory_super!(CEntity);

    memory_field!(transform: CTransform; @ 0x34);
}

memory_object!(CPhysicsActor);
impl CPhysicsActor {
    memory_super!(CActor);

    memory_field!(constantForce: CVector3f; @ 0xFC);
    memory_field!(angularMomentum: CVector3f; @ 0x108);
    memory_field!(velocity: CVector3f; @ 0x138);
    memory_field!(angularVelocity: CVector3f; @ 0x144);
    memory_field!(momentum: CVector3f; @ 0x150);
    memory_field!(force: CVector3f; @ 0x15C);
    memory_field!(impulse: CVector3f; @ 0x168);
    memory_field!(torque: CVector3f; @ 0x174);
    memory_field!(angularImpulse: CVector3f; @ 0x180);
    memory_field!(collisionPrimitive: CAABBPrimitive; @ 0x1c0);
    memory_field!(primitiveOffset: CVector3f; @ 0x1E8);
    memory_field!(translation: CVector3f; @ 0x1F4);
    memory_field!(orientation: CQuaternion; @ 0x200);
}

memory_object!(CPlayer);
impl CPlayer {
    memory_super!(CPhysicsActor);
}
