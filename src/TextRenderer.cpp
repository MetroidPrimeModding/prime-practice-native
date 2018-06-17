#include "include/TextRenderer.hpp"
#include "VGA-ROM-L4.h"
#include <string.h>
#include "include/GX.hpp"
#include "include/prime/CGraphics.hpp"

#define WHITE (0xFFFF8080)
#define BLACK (0x00008080)
#define LEFT_WHITE_RIGHT_BLACK (0xFF008080)
#define LEFT_BLACK_RIGHT_WHITE (0x00FF8080)

#define CHAR_TEX_OFFSET (CHAR_TEX_DIM)
#define CHAR_DIM_X (CHAR_DIM)
#define CHAR_DIM_Y (CHAR_DIM + 0.5f)

// Various things needed for GX
bool initialized = false;
char *fontTexData;
GXTexObj fontTexture;
float colorR = 1, colorG = 1, colorB = 1, colorA = 1;

void TextRenderer::RenderText(const char *str, float xStart, float yStart) {
  if (str == nullptr) {
    return;
  }
  u32 len = strlen(str);
  float x = xStart;
  float y = yStart;

  // This is the max characters that actually works per render, for some reason
  // 30 quads, or 154 verts, or 1386 floats, or 5544 bytes
  // Not sure why that's the limit...
  int charsMaxPerRender = 30;
  int charsRendered = 0;

  CGraphics::StreamBegin(ERglPrimitive_QUADS);
  CGraphics::StreamColor(colorR, colorG, colorB, colorA);
  for (u32 i = 0; i < len; i++) {
    char c = str[i];
    if (c == '\n') {
      y += CHAR_DIM + LINE_SPACING;
      x = xStart;
      continue;
    }
    if (x > 640 - CHAR_DIM) {
      x = xStart;
      y += CHAR_DIM + LINE_SPACING;
    }

    if (charsRendered >= charsMaxPerRender) {
      charsRendered = 0;
      CGraphics::FlushStream();
    }

    // Don't render spaces...
    if (c != ' ') {
      RenderChar((u8)c, x, y);
      charsRendered++;
    }

    x += CHAR_DIM;
  }
  CGraphics::StreamEnd();
}

void TextRenderer::RenderChar(u8 c, float xStart, float yStart) {
  int row = (c / TEX_CHARS_WIDE);
  int col = (c % TEX_CHARS_WIDE);

  float s = CHAR_TEX_DIM * col;
  float t = CHAR_TEX_DIM * row;

  CGraphics::StreamTexcoord(s, t);
  CGraphics::StreamVertex(xStart + CHAR_DIM_X, 0, yStart);

  CGraphics::StreamTexcoord(s, t + CHAR_TEX_OFFSET);
  CGraphics::StreamVertex(xStart + CHAR_DIM_X, 0, yStart + CHAR_DIM_Y);

  CGraphics::StreamTexcoord(s + CHAR_TEX_OFFSET, t + CHAR_TEX_OFFSET);
  CGraphics::StreamVertex(xStart, 0, yStart + CHAR_DIM_Y);

  CGraphics::StreamTexcoord(s + CHAR_TEX_OFFSET, t);
  CGraphics::StreamVertex(xStart, 0, yStart);
}

void TextRenderer::Init() {
  if (initialized) {
    return;
  }
  initialized = true;
  // Have to force alignment, relocation breaks alignment right now
  fontTexData = new char[VGA_ROM_l4_len];
  memcpy(fontTexData, VGA_ROM_l4, VGA_ROM_l4_len);
  GXInitTexObj(&fontTexture, fontTexData,
               128, 128,
               GX_TF_I4,
               GX_CLAMP, GX_CLAMP,
               GX_FALSE
  );
  GXInitTexObjLOD(&fontTexture,
                  GX_NEAR, GX_NEAR,
                  0, 0,
                  0,
                  GX_DISABLE,
                  GX_DISABLE,
                  GX_ANISO_1
  );
}

void TextRenderer::SetColor(float r, float g, float b, float a) {
  colorR = r;
  colorG = g;
  colorB = b;
  colorA = a;
}

