//#include <cstdio>
//#include "TextRenderer.hpp"
//#include "prime/CGameAllocator.hpp"
//#include "UI/UIHeapUsageElement.hpp"
//
//void UIHeapUsageElement::draw() {
//  if (hidden) { return; }
//  UIElement::draw();
//
//  CGameAllocator *allocator = CGameAllocator_LOCATION;
//
//  u32 freeCount = 0;
//  u32 freeSize = 0;
//  u32 usedCount = 0;
//  u32 usedSize = 0;
//
//  for (CMemoryBlock *block = allocator->first(); block != nullptr; block = block->next) {
//    if (freeCount + usedCount > 10000 || !VALID_PTR(block)) {
//      break; // Don't infinite loop
//    }
//    if (block->dataStart == 0) {
//      freeCount++;
//      freeSize += block->size + 32;
//    } else {
//      usedCount++;
//      usedSize += block->size + 32;
//    }
//  }
//
//  u32 totalHeapSize = usedSize + freeSize;
//
//  u32 usedPercent = usedSize * 100 / totalHeapSize;
//  u32 freePrecent = freeSize * 100 / totalHeapSize;
//
//  if (freeSize < minFree) {
//    minFree = freeSize;
//  }
//
//  if (usedPercent > peakPercent) {
//    peakPercent = usedPercent;
//  }
//
//  char buff[128];
//  snprintf(buff, 128, "%2u%%/%5uk Used %2u%%/%5uk Free %uk Total %2u%%/%5uk Peak",
//               usedPercent, usedSize / 1024,
//               freePrecent, freeSize / 1024,
//               totalHeapSize / 1024,
//               peakPercent, minFree / 1024);
//  TextRenderer::RenderText(buff, x, y);
//}
