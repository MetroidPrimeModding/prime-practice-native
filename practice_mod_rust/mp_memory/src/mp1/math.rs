use crate::game_types::Constructable;
use crate::game_types::Float32;

memory_object!(CTransform);
impl CTransform {
    memory_field!(array matrix: Float32; @ 0x0; size 12);
}
sized_constructable!(CTransform size 4 * 12);

memory_object!(CVector3f);
impl CVector3f {
    memory_field!(array raw: Float32; @ 0x0; size 3);
    memory_field!(x: Float32; @ 0x0);
    memory_field!(y: Float32; @ 0x4);
    memory_field!(z: Float32; @ 0x8);
}
sized_constructable!(CVector3f size 4 * 3);

memory_object!(CQuaternion);
impl CQuaternion {
    memory_field!(array raw: Float32; @ 0x0; size 4);
    memory_field!(x: Float32; @ 0x0);
    memory_field!(y: Float32; @ 0x4);
    memory_field!(z: Float32; @ 0x8);
    memory_field!(w: Float32; @ 0xC);
}
sized_constructable!(CQuaternion size 4 * 4);

memory_object!(CAABBPrimitive);
impl CAABBPrimitive {
    memory_field!(aabb: CAABB; @ 0x10);
}

memory_object!(CAABB);
impl CAABB {
    memory_field!(min: CVector3f; @ 0x0);
    memory_field!(max: CVector3f; @ 0xC);
}
sized_constructable!(CAABB size 4 * 6);
