#include "CrashAssert.h"
#include "os.h"

extern "C" {
void CrashAssert(bool v) {
  if (!v) {
    OSReport("ASSERTION FAILED");
    int i = 0;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
      i++; // hang
    }
#pragma clang diagnostic pop
  }
}
}
