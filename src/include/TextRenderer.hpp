#ifndef PRIME_PRACTICE_TEXTRENDERER_HPP
#define PRIME_PRACTICE_TEXTRENDERER_HPP

#include "PrimeAPI.h"
#include "types.h"
#include "GX.hpp"

#define TEX_CHARS_WIDE 16
#define CHAR_TEX_DIM (1.0f/16.0f)
#define CHAR_DIM (8.0f)
#define LINE_SPACING (2.0f)

extern GXTexObj fontTexture;
extern GXTexObj imguiFontTexture;


class TextRenderer {
public:
    static void Init();
    static void SetColor(float r, float g, float b, float a);
    static void RenderText(const char *str, int x, int y);
    static void RenderText(const char *str, float x, float y);
    static void RenderText(const char *str, u32 len, float x, float y);
    static void RenderChar(u8 c, f32 x, f32 y);
};

#endif //PRIME_PRACTICE_TEXTRENDERER_HPP
