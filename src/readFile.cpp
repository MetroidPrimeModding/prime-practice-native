#include <string.h>
#include "include/readFile.hpp"

#include <ogcsys.h>
#include <include/dvd.h>
#include <include/os.h>

bool reading = false;
s32 readBytes = 0;

void finishedReadingCallback(s32 result, DVDFileInfo* fileInfo);

ReadFileResult readFileSync(const char *path) {
  if (reading) {
    reading = *((bool*)0xDEADBEE2);
  }
  reading = false;

  DVDFileInfo info;

  char *tmpPath = new char[strlen(path)];
  memcpy(tmpPath, path, strlen(path) + 1);
  DVDOpen(tmpPath, &info);
  u32 len = DVDGetLength(&info);
  if (len % 32 > 0) {
    len += 32 - (len % 32);
  }

  void *bytes = new char[len];

  reading = true;
  DVDReadAsyncPrio(&info, bytes, len, 0, &finishedReadingCallback, 2);

  while (reading) {
    OSYieldThread();
  }

  DVDClose(&info);

  if (readBytes != len) {
    reading = *((bool*)0xDEADBEE3);
    reading = false;
  }

  ReadFileResult res;
  res.data = bytes;
  res.len = len;

  delete tmpPath;

  return res;
}

void finishedReadingCallback(s32 result, DVDFileInfo *fileInfo) {
  reading = false;
  readBytes = result;
}
