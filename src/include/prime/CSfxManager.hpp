#ifndef PRIMEWATCH_PRIME_CSFXMANAGER_HPP
#define PRIMEWATCH_PRIME_CSFXMANAGER_HPP

class CSfxHandle;
class CSfxHandle {
public:
    static u32 refCount;
    s32 unk1;
    s32 unk2; //ID?
    s32 unk3;
};
class CSfxManager;
class CSfxManager {
public:
    static CSfxHandle SfxStart(u16 id, s16 vol, s16 pan, bool useAcoustics, s16 prio, bool looped, int areaId);
};

#endif //PRIMEWATCH_PRIME_CSFXMANAGER_HPP
