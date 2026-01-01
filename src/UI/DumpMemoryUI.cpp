#include "DumpMemoryUI.hpp"
#include "../system/ImGuiEngine.hpp"
#include "GX.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "os.h"
#include "utils/DataByteStream.hpp"

namespace GUI {
  constexpr int pixelsPerBlock = 4;
  // texture needs to be a multiple of 32 bytes (specifically 8x4 blocks)
  constexpr int deadzone_x = 16;
  constexpr int deadzone_y = 16;
  constexpr int tex_width = (640 - deadzone_x * 2) / pixelsPerBlock;
  constexpr int tex_height = (448 - deadzone_y * 2) / pixelsPerBlock;
  constexpr int tex_size = tex_width * tex_height; //1 byte per pixel
  constexpr int display_duration = 2; // frames

  DataByteStream dataByteStream{nullptr, 0};
  int displayedFrames = 0;

#define RGB565(r, g, b) ((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3))
  alignas(32) const u16 colors[16] = {
      RGB565(0x00, 0x00, 0x00), // black
      RGB565(0x00, 0x00, 0xFF), // blue
      RGB565(0x00, 0xFF, 0x00), // green
      RGB565(0x00, 0xFF, 0xFF), // cyan
      RGB565(0xFF, 0x00, 0x00), // red
      RGB565(0xFF, 0x00, 0xFF), // magenta
      RGB565(0xFF, 0xFF, 0x00), // yellow
      RGB565(0xFF, 0xFF, 0xFF), // white
      RGB565(0x80, 0x80, 0x80), // gray
      RGB565(0x00, 0x00, 0x80), // dark blue
      RGB565(0x00, 0x80, 0x00), // dark green
      RGB565(0x00, 0x80, 0x80), // dark cyan
      RGB565(0x80, 0x00, 0x00), // dark red
      RGB565(0x80, 0x00, 0x80), // dark magenta
      RGB565(0x80, 0x80, 0x00), // dark yellow
      RGB565(0xC0, 0xC0, 0xC0)  // light gray
  };
  static GXTlutObj gxTlutObj;
  static GXTexObj gxTexObj;
  static IMGuiGXTexture imGuiTexture;

  char *textureData = nullptr;

  void startMemoryDump() {
    // first init the texture
    if (textureData != nullptr) {
      delete textureData;
    }
    textureData = new char[tex_size];
    // verify 32-byte alignment
    if ((u32) textureData % 32 != 0) {
      DebugLog("Texture data not 32-byte aligned!!!\n");
      delete textureData;
      return;
    }

    GXInitTlutObj(&gxTlutObj, (void *) colors, GX_TL_RGB565, 16);

    memset(textureData, 0, tex_size);
    GXInitTexObjCI(
        &gxTexObj,
        (void *) textureData,
        tex_width, tex_height,
        GX_TF_CI8,
        GX_CLAMP, GX_CLAMP,
        GX_FALSE,
        GX_TLUT15
    );
    GXInitTexObjLOD(
        &gxTexObj,
        GX_NEAR, GX_NEAR,
        0, 0,
        0,
        GX_DISABLE,
        GX_DISABLE,
        GX_ANISO_1
    );
    imGuiTexture.obj = &gxTexObj;
    imGuiTexture.tlut = &gxTlutObj;
    imGuiTexture.tlut_name = GX_TLUT15;

    //and set our start pointer
    dataByteStream = DataByteStream{(void *) 0x80000000, 0x1800000};
    displayedFrames = display_duration;
  }

  void endMemoryDump() {
    // clean up the texture
    if (textureData != nullptr) {
      delete textureData;
      textureData = nullptr;
    }
  }

  void updateTexture();

  void drawMemoryDump() {
    if (textureData == nullptr || dataByteStream.eof()) {
      endMemoryDump();
      return;
    }

    if (displayedFrames > display_duration) {
      updateTexture();
      displayedFrames = 0;
    }
    displayedFrames++;

    ImGui::SetNextWindowPos(ImVec2(deadzone_x, deadzone_y), ImGuiCond_None, ImVec2(0, 0));
    ImGui::SetNextWindowFocus();
    ImGui::Begin(
        "Memory Dump", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNavInputs |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiFocusedFlags_None // just for conveneint commenting in/out
    );
    ImGui::Image(
        (void *) &imGuiTexture,
        ImVec2(tex_width * pixelsPerBlock, tex_height * pixelsPerBlock),
        ImVec2(0, 0),
        ImVec2(1, 1)
    );
    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float completionPercentage = (float) dataByteStream.getOffset() / (float) dataByteStream.getSize();
    float barWidth = tex_width * pixelsPerBlock;
    float barHeight = 16;
    dl->AddRectFilled(
        p,
        ImVec2(p.x + barWidth * completionPercentage, p.y + barHeight),
        IM_COL32(255, 255, 255, 255)
    );
    dl->AddRectFilled(
        ImVec2(p.x + barWidth * completionPercentage, p.y),
        ImVec2(p.x + barWidth, p.y + barHeight),
        IM_COL32(0, 0, 0, 255)
    );
    ImGui::Dummy(ImVec2(tex_width * pixelsPerBlock, tex_height * pixelsPerBlock));
    ImGui::End();
  }

  void convertTextureToBlocks(char *input);

  void updateTexture() {
    u32 startOffset = dataByteStream.getOffset();
    dataByteStream.resetCrc();

    char *tempData = new char[tex_width * tex_height];
    memset(tempData, 0, tex_width * tex_height);

    // first copy in our data - the edges will be calibration data
    for (int y = 1; y < tex_height - 1; y++) {
      for (int x = 1; x < tex_width - 1; x += 1) {
//        tempData[y * tex_width + x] = x % 16;
        u8 byte;
        dataByteStream.getBytes(&byte, 1);
        tempData[y * tex_width + x] = byte >> 4;
        tempData[y * tex_width + x + 1] = byte & 0x0F;
      }
    }

    // build a calibration/metadata row
    char calibrationRow[tex_width] = {};
    {
      int x = 0;
      // first a calibration pattern of black and white squares across the whole thing
      for (int i = 0; i < tex_width; i++) {
        calibrationRow[x++] = i % 2 ? 0 : 7;
      }
      x =                 7;
      // then a color spectrum
      for (int i = 0; i < 16; i++) {
        calibrationRow[x++] = i;
      }
      x += 5;
      // then the starting address
      for (int i = 0; i < 8; i++) {
        calibrationRow[x++] = (startOffset >> (30 - i * 4)) & 0xF;
      }
      x += 5;
      // then the CRC of the data on screen
      u32 crc = dataByteStream.getCrc();
      for (int i = 0; i < 8; i++) {
        calibrationRow[x++] = (crc >> (28 - i * 4)) & 0xF;
      }
      // copy the first x bytes to the end (minus 1 since it's even width)
      for (int i = 0; i < x; i++) {
        calibrationRow[tex_width - 2 - i] = calibrationRow[i];
      }
    }

    // row 0 & row N - 1 = calibration
    for (int x = 0; x < tex_width; x++) {
      tempData[x] = calibrationRow[x];
      tempData[(tex_height - 1) * tex_width + x] = calibrationRow[x];
    }
    // column 1 & column N - 1 = calibration
    for (int y = 0; y < tex_height; y++) {
      tempData[y * tex_width] = calibrationRow[y];
      tempData[y * tex_width + tex_width - 1] = calibrationRow[y];
    }

    // TODO: better error correction code encoding here?

    convertTextureToBlocks(tempData);
    delete[] tempData;
  }

  void convertTextureToBlocks(char *input) {
    // The gamecube stores textures in 32 byte blocks
    // in this case, that is 8x4

    constexpr int blockWidth = 8;
    constexpr int blockHeight = 4;
    const int xBlocks = tex_width / blockWidth;
    const int yBlocks = tex_height / blockHeight;

    for (int xBlock = 0; xBlock < xBlocks; xBlock++) {
      for (int yBlock = 0; yBlock < yBlocks; yBlock++) {
        int blockStartX = xBlock * blockWidth;
        int blockStartY = yBlock * blockHeight;
        int outStart = ((yBlock * xBlocks) + xBlock) * (blockWidth * blockHeight);
        // swizzle the block
        for (int blockRelativeX = 0; blockRelativeX < blockWidth; blockRelativeX++) {
          for (int blockRelativeY = 0; blockRelativeY < blockHeight; blockRelativeY++) {
            int blockRelativeOffset = blockRelativeY * blockWidth + blockRelativeX;
            int outOffset = outStart + blockRelativeOffset;

            int x = blockStartX + blockRelativeX;
            int y = blockStartY + blockRelativeY;
            int inOffset = y * tex_width + x;
            textureData[outOffset] = input[inOffset];
          }
        }
        // Done with the block
      }
    }
  }

}
