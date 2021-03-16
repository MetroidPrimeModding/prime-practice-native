#ifndef PRIME_PRACTICE_CMEMORYCARDSYS_HPP
#define PRIME_PRACTICE_CMEMORYCARDSYS_HPP

#include "prime/CStateManager.hpp"

class CSaveWorldIntermediate;
class CSaveWorld;

class CMemoryCardSys {
public:
    PADDING(0x18);
    CSaveWorldIntermediate *worldIntermediate;
};

class CSaveWorldIntermediate {
public:
    uint32 mlvlID; //4
    PADDING(0x24);
    rstl::vector<CWorldLayers::Area> defaultLayerStates;
//    CWorldLayers::Area *defaultLayerStates;
    PADDING(0x14);
};

class CSaveWorld {
public:
    PADDING(0x1c);


    struct SScanState {
    };
};

#endif //PRIME_PRACTICE_CMEMORYCARDSYS_HPP
