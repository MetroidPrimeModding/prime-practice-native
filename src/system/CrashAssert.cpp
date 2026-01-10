#include "CrashAssert.h"
#include "os.h"

extern "C" {
void CrashAssert(bool v, const char* file, int line) {
  if (!v) {
    OSReport("ASSERTION FAILED: %s:%d\n", file, line);
    while (true);
  }
}

void abort() {
  OSReport("ABORT!\n");
  while (true);
}
}
