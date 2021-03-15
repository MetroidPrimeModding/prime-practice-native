#ifndef PRIMEWATCH_PRIME_CGRAPHICS_HPP
#define PRIMEWATCH_PRIME_CGRAPHICS_HPP

#include "MathyTypes.hpp"
#include "GX.hpp"

enum ERglCullMode {
    ERglCullMode_None = 0,
    ERglCullMode_Front = 1,
    ERglCullMode_Back = 2,
    ERglCullMode_All = 3
};

enum ERglPrimitive {
    ERglPrimitive_POINTS = 0xB8,
    ERglPrimitive_LINES = 0xA8,
    ERglPrimitive_LINESTRIP = 0xB0,
    ERglPrimitive_TRIANGLES = 0x90,
    ERglPrimitive_TRIANGLESTRIP = 0x98,
    ERglPrimitive_TRIANGLEFAN = 0xA0,
    ERglPrimitive_QUADS = 0x80
};

enum ERglAlphaFunc {
    ERglAlphaFunc_NEVER = 0,
    ERglAlphaFunc_LESS = 1,
    ERglAlphaFunc_EQUAL = 2,
    ERglAlphaFunc_LEQUAL = 3,
    ERglAlphaFunc_GREATER = 4,
    ERglAlphaFunc_NEQUAL = 5,
    ERglAlphaFunc_GEQUAL = 6,
    ERglAlphaFunc_ALWAYS = 7
};

enum ERglAlphaOp {
    ERglAlphaOp_AND = 0,
    ERglAlphaOp_OR = 1,
    ERglAlphaOp_XOR = 2,
    ERglAlphaOp_XNOR = 3
};


#define SVIEWPORT_GLOBAL ((SViewport*) 0x803ED910)
struct SViewport {
    u32 x0_left;
    u32 x4_top;
    u32 x8_width;
    u32 xc_height;
    float x10_halfWidth;
    float x14_halfHeight;
};

class CGraphics {
public:
    static void SetCullMode(ERglCullMode mode);
    static void SetOrtho(float left, float right, float top, float bottom, float near, float far);
    static void SetViewPointMatrix(const CTransform4f &mat);
    static void SetModelMatrix(const CTransform4f &mat);
    static void SetIdentityModelMatrix();
    static void SetIdentityViewPointMatrix();
    static void VideoPreCallback(ulong arg);
    static void EndScene();
    static void BeginScene();
    static void SetDefaultVtxAttrFmt();
    static void InitGraphicsDefaults();
    static void StreamBegin(ERglPrimitive primitive);
    static void StreamEnd();
    static void FlushStream();
    static void StreamColor(float r, float g, float b, float a);
    static void StreamVertex(float x, float y, float z);
    static void StreamTexcoord(float s, float t);
    static void StreamVertex(const CVector3f &vec);
    static void StreamNormal(const CVector3f &vec);
    static float GetFPS();
    static void SetAlphaCompare(ERglAlphaFunc func0, u8 ref0, ERglAlphaOp op, ERglAlphaFunc func1, u8 ref1);
    static void DisableAllLights();
};

class CGX {
public:
    static void SetNumTevStages(u8 num);
    static void SetTevOrder(_GXTevStageID, _GXTexCoordID, _GXTexMapID, _GXChannelID);
    static void SetZMode(bool, _GXCompare, bool);
    static void SetTevColorIn(_GXTevStageID, _GXTevColorArg, _GXTevColorArg, _GXTevColorArg, _GXTevColorArg);
    static void SetTevAlphaIn(_GXTevStageID, _GXTevAlphaArg, _GXTevAlphaArg, _GXTevAlphaArg, _GXTevAlphaArg);
    static void SetTevColorOp(_GXTevStageID, _GXTevOp, _GXTevBias, _GXTevScale, u8, _GXTevRegID);
    static void SetTevAlphaOp(_GXTevStageID, _GXTevOp, _GXTevBias, _GXTevScale, u8, _GXTevRegID);
    static void SetBlendMode(_GXBlendMode, _GXBlendFactor, _GXBlendFactor, _GXLogicOp);
};

#endif //PRIMEWATCH_PRIME_CGRAPHICS_HPP
