#include <ogc/dvd.h>

typedef short fb_t;

extern void OSResetSystem(int);
extern void OSInit();
extern void VISetNextFrameBuffer(void *);
extern void VISetBlack(int);
extern void VIFlush();
extern void VIInit();
extern void VIWaitForRetrace();
extern int OSGetArenaHi();
extern int OSGetArenaLo();

extern void *__payload_end;

#define true 1
#define false 0
#define NULL 0
#define __printflike(fmtarg, firstvararg) \
    __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
extern int printf(const char *__restrict, ...) __printflike(1, 2);

fb_t *fb1 = (fb_t *) 0x804f0c00;
fb_t *fb2 = (fb_t *) 0x80586c00;

void _payload() {
  *((int *) 0x8045D850) = 0xFFFFFFFF;

  int fb = 1;

  for (int i = 0; i < 480 * 640; i++) {
    fb1[i] = (fb_t) 0xDDDD;
    fb2[i] = (fb_t) 0xDDDD;
  }

  printf("Resetting @ %x %x", OSGetArenaLo(), OSGetArenaHi());
  VISetNextFrameBuffer(fb1);
  VIFlush();
  VIWaitForRetrace();
  OSResetSystem(2);
  OSInit();
  VIInit();
  VISetBlack(0);
  printf("All set up @ %x %x", OSGetArenaLo(), OSGetArenaHi());
  int lastStatus = -1;
  char lastID[5];
  lastID[4] = 0;
  bool lastChecked = 5;
//  DVDFileInfo fi;
  bool opened = false;
  DVD_Init();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
  while (1) {

//
//    if (!opened) {
//      s32 path = DVDConvertPathToEntrynum("default.dol");
//      if (path != -1) {
//        opened = DVDFastOpen(path, &fi);
//        if (opened) {
//          printf("Opened file");
//        }
//      }
//    }
//
//    bool checked = DVDCheckDisk();
//    if (lastChecked != checked) {
//      printf("DVD checked: %s", checked ? "TRUE" : "FALSE");
//    }
//
//    // wait for disc to swap
//    int status = DVDGetDriveStatus();
//    if (status != lastStatus) {
//      lastStatus = status;
//      printf("DVD status: %d", status);
//    }
//    struct DVDDiskID *id = DVDGetCurrentDiskID();
//    if (id == NULL) {
//      lastID[0] = '-';
//      lastID[1] = '-';
//      lastID[2] = '-';
//      lastID[3] = '-';
//      printf("ID: %s", lastID);
//    } else {
//      bool matches = true;
//      for (int i = 0; i < 4; i++) {
//        if (lastID[i] != id->gameName[i]) {
//          matches = false;
//        }
//        lastID[i] = id->gameName[i];
//      }
//      if (!matches) {
//        printf("ID: %s", lastID);
//      }
//    }
//    VIWaitForRetrace();
  }
#pragma clang diagnostic pop


  //hang
//  while (1) {};
  asm("b return_to_smash");
}