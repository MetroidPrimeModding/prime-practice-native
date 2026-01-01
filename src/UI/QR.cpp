#include "QR.hpp"
#include "../system/ImGuiEngine.hpp"
#include "GX.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "os.h"
#include "qrcode.h"

namespace GUI {
  constexpr int TEX_COUNT = 2;
  constexpr int QR_WIDTH = LOCK_VERSION * 4 + 17;
  constexpr int MAX_WIDTH = 40;
  constexpr int TEX_SIZE = (MAX_WIDTH * MAX_WIDTH) / 2;

  static int usedTextures[TEX_COUNT];
  alignas(32) static char qrTextureData[TEX_COUNT][TEX_SIZE];
  static ImGuiID texIds[TEX_COUNT];
  static GXTexObj gxTexObjs[TEX_COUNT];
  static IMGuiGXTexture imGuiTextures[TEX_COUNT] = {};

  void initQR() {
    for (int i = 0; i < TEX_COUNT; i++) {
      usedTextures[i] = false;
      texIds[i] = 0;
      memset(&qrTextureData[i * TEX_SIZE], 0, TEX_SIZE);
      GXInitTexObj(&gxTexObjs[i], (void *) &qrTextureData[i],
                   MAX_WIDTH, MAX_WIDTH,
                   GX_TF_I4,
                   GX_CLAMP, GX_CLAMP,
                   GX_FALSE
      );
      GXInitTexObjLOD(&gxTexObjs[i],
                      GX_NEAR, GX_NEAR,
                      0, 0,
                      0,
                      GX_DISABLE,
                      GX_DISABLE,
                      GX_ANISO_1
      );
      imGuiTextures[i].obj = &gxTexObjs[i];
      imGuiTextures[i].tlut = nullptr;
      imGuiTextures[i].tlut_name = GX_TLUT0;
    }
  }

  void qrNewFrame() {
    for (int i = 0; i < TEX_COUNT; i++) {
      usedTextures[i] = false;
    }
  }

  int setUpTexture(const char *data, QR_ECC ecc) {
    const ImGuiID id = ImGui::GetID(data);

    for (int i = 0; i < TEX_COUNT; i++) {
      if (texIds[i] == id) {
        usedTextures[i] = true;
        return i;
      }
    }

    // look for an unused texture
    int qrNum = -1;
    for (int i = 0; i < TEX_COUNT; i++) {
      if (!usedTextures[i]) {
        qrNum = i;
        break;
      }
    }
    if (qrNum < 0) return qrNum;
    usedTextures[qrNum] = true;
    texIds[qrNum] = id;

    DebugLog("Rendering QR %s id %u\n", data, id);
    QRCode qrcode;
    uint8_t qrcodeBytes[qrcode_getBufferSize(LOCK_VERSION)];
    qrcode_initText(&qrcode, qrcodeBytes, LOCK_VERSION, (int) ecc, data);
    // copy data over
    // gx stores textures weirdly
    constexpr int blockWidth = 8;
    constexpr int blockHeight = 8;
    const int xBlocks = MAX_WIDTH / blockWidth;
    const int yBlocks = MAX_WIDTH / blockHeight;

    for (int xBlock = 0; xBlock < xBlocks; xBlock++) {
      for (int yBlock = 0; yBlock < yBlocks; yBlock++) {
        int blockStartX = xBlock * blockWidth;
        int blockStartY = yBlock * blockHeight;
        int outStart = ((yBlock * xBlocks) + xBlock) * (blockWidth * blockHeight);
        // Ok now loop over the pixels
        for (int blockRelativeX = 0; blockRelativeX < blockWidth; blockRelativeX++) {
          for (int blockRelativeY = 0; blockRelativeY < blockHeight; blockRelativeY++) {
            int blockRelativeOffset = blockRelativeY * blockWidth + blockRelativeX;
            unsigned char pixel = 0x0;
            if (qrcode_getModule(&qrcode, blockStartX + blockRelativeX, blockStartY + blockRelativeY)) {
              pixel = 0xF;
            }
            int pixelPos = outStart + blockRelativeOffset;
            int bytePos = pixelPos / 2;
            if (pixelPos % 2 == 0) {
              qrTextureData[qrNum][bytePos] = qrTextureData[qrNum][bytePos] & 0xF | (pixel << 4);
            } else {
              qrTextureData[qrNum][bytePos] = qrTextureData[qrNum][bytePos] & 0xF0 | pixel;
            }
          }
        }
        // Done with the block
      }
    }

    return qrNum;
  }

  void drawQRCode(const char *data, float sizeMultiplier, QR_ECC ecc) {
    int qrNum = setUpTexture(data, ecc);
    if (qrNum < 0) {
      ImGui::Text("Only %d QR codes may be displayed at once", TEX_COUNT);
      return;
    }
//    ImGuiIO &io = ImGui::GetIO();
//    ImDrawList *dl = ImGui::GetWindowDrawList();
//    ImVec2 p = ImGui::GetCursorScreenPos() + ImVec2(sizeMultiplier, sizeMultiplier);
    ImGui::Image(
        (ImTextureID) &imGuiTextures[qrNum],
        ImVec2(MAX_WIDTH * sizeMultiplier, MAX_WIDTH * sizeMultiplier),
        ImVec2(0, 0),
        ImVec2((float) QR_WIDTH / (float) MAX_WIDTH, (float) QR_WIDTH / (float) MAX_WIDTH)
    );
  }
}
