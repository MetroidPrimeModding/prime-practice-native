#pragma once

#include "CWorldState.hpp"
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
  uint32 mlvlID; // 4
  PADDING(0x24);
  rstl::vector<CWorldLayers::Area> defaultLayerStates;
  //    CWorldLayers::Area *defaultLayerStates;
  PADDING(0x14);
};

class CSaveWorld {
public:
  PADDING(0x1c);

  struct SScanState {};
};

enum class EMCState {
  Invalid = -0x1,
  Uninitialized = 0x0,
  Idle = 0x1,
  NoCardFound = 0x2,
  WriteBackupBuf = 0x3,
  Formatted = 0x4,
  CardProbed = 0x5,
  MountCard = 0x6,
  CardCheck = 0x7,
  FileCreated = 0x8,
  FileCreatedTransactional = 0x9,
  TransferComplete = 0xA,
  FileAltDeleted = 0xB,
  ErrorMounting = 0xC,
  ErrorOutOfSpace = 0xE,
  ErrorNoFile = 0x11,
  ErrorCardFullOnCreate = 0x12,
  ErrorUpdateFileWrite = 0x13,
  ErrorCardFullOnCreateTransactional = 0x14,
  ErrorUpdateWriteTransactional = 0x15,
  ErrorFailedCardFormat = 0x18,
  StartCardProbe = 0x19,
  StartMountCard = 0x1A,
  StartCheckCard = 0x1B,
  StartFileDeleteBad = 0x1C,
  StartFileRead = 0x1D,
  StartFileDeleteAlt = 0x1E,
  StartFileCreate = 0x1F,
  StartFileWrite = 0x20,
  StartFileCreateTransactional = 0x21,
  StartDeleteAltTransactional = 0x23,
  StartAtoB = 0x24,
  StartCardFormat = 0x25,

  // CUSTOM STATES FOR PRAC MOD
  StartPracModRead = 0x100001,
  StartPracModWrite = 0x100002,
};

class CMemoryCardDriver {
public:
  void Update(float dt);
  void StartFileWrite();

  s32 getCardPort() { return *GetField<s32>(this, 0x0); }
  EMCState *getState() { return GetField<EMCState>(this, 0x10); }
  s32 *getError() { return GetField<s32>(this, 0x14); }
  s32 getCardFreeBytes() { return *GetField<s32>(this, 0x18); }
  s32 getCardFreeFiles() { return *GetField<s32>(this, 0x1C); }
  u32 getFileTimeStamp() { return *GetField<u64>(this, 0x20); }
  u64 getCardSerialNumber() { return *GetField<u64>(this, 0x28); }

};
