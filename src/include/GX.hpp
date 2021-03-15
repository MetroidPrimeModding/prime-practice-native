#ifndef PRIME_PRACTICE_GX_HPP
#define PRIME_PRACTICE_GX_HPP

#include "types.h"

#define GX_TF_I4      0x0
#define GX_TF_I8      0x1
#define GX_TF_IA4      0x2
#define GX_TF_IA8      0x3
#define GX_TF_RGB565    0x4
#define GX_TF_RGB5A3    0x5
#define GX_TF_RGBA8      0x6
#define GX_TF_CI4      0x8
#define GX_TF_CI8      0x9
#define GX_TF_CI14      0xa
#define GX_TF_CMPR      0xE

#define GX_CLAMP      0
#define GX_REPEAT      1
#define GX_MIRROR      2

#define GX_FALSE      0
#define GX_TRUE        1
#define GX_DISABLE      0
#define GX_ENABLE      1

#define GX_POINTS        0xB8
#define GX_LINES        0xA8
#define GX_LINESTRIP      0xB0
#define GX_TRIANGLES      0x90
#define GX_TRIANGLESTRIP    0x98
#define GX_TRIANGLEFAN      0xA0
#define GX_QUADS        0x80

#define GX_BL_ZERO      0
#define GX_BL_ONE      1
#define GX_BL_SRCCLR    2
#define GX_BL_INVSRCCLR    3
#define GX_BL_SRCALPHA    4
#define GX_BL_INVSRCALPHA  5
#define GX_BL_DSTALPHA    6
#define GX_BL_INVDSTALPHA  7
#define GX_BL_DSTCLR    GX_BL_SRCCLR
#define GX_BL_INVDSTCLR    GX_BL_INVSRCCLR

#define GX_BM_NONE      0
#define GX_BM_BLEND      1
#define GX_BM_LOGIC      2
#define GX_BM_SUBTRACT    3
#define GX_MAX_BLENDMODE  4

#define GX_LO_CLEAR      0
#define GX_LO_AND      1
#define GX_LO_REVAND    2
#define GX_LO_COPY      3
#define GX_LO_INVAND    4
#define GX_LO_NOOP      5
#define GX_LO_XOR      6
#define GX_LO_OR      7
#define GX_LO_NOR      8
#define GX_LO_EQUIV      9
#define GX_LO_INV      10
#define GX_LO_REVOR      11
#define GX_LO_INVCOPY    12
#define GX_LO_INVOR      13
#define GX_LO_NAND      14
#define GX_LO_SET      15

#define GX_NEVER      0
#define GX_LESS        1
#define GX_EQUAL      2
#define GX_LEQUAL      3
#define GX_GREATER      4
#define GX_NEQUAL      5
#define GX_GEQUAL      6
#define GX_ALWAYS      7

#define GX_VTXFMT0      0
#define GX_VTXFMT1      1
#define GX_VTXFMT2      2
#define GX_VTXFMT3      3
#define GX_VTXFMT4      4
#define GX_VTXFMT5      5
#define GX_VTXFMT6      6
#define GX_VTXFMT7      7
#define GX_MAXVTXFMT    8

#define GX_VA_PTNMTXIDX      0
#define GX_VA_TEX0MTXIDX    1
#define GX_VA_TEX1MTXIDX    2
#define GX_VA_TEX2MTXIDX    3
#define GX_VA_TEX3MTXIDX    4
#define GX_VA_TEX4MTXIDX    5
#define GX_VA_TEX5MTXIDX    6
#define GX_VA_TEX6MTXIDX    7
#define GX_VA_TEX7MTXIDX    8
#define GX_VA_POS        9
#define GX_VA_NRM        10
#define GX_VA_CLR0        11
#define GX_VA_CLR1        12
#define GX_VA_TEX0        13
#define GX_VA_TEX1        14
#define GX_VA_TEX2        15
#define GX_VA_TEX3        16
#define GX_VA_TEX4        17
#define GX_VA_TEX5        18
#define GX_VA_TEX6        19
#define GX_VA_TEX7        20
#define GX_POSMTXARRAY      21
#define GX_NRMMTXARRAY      22
#define GX_TEXMTXARRAY      23
#define GX_LIGHTARRAY      24
#define GX_VA_NBT        25
#define GX_VA_MAXATTR      26
#define GX_VA_NULL        0xff

#define GX_NONE        0
#define GX_DIRECT      1
#define GX_INDEX8      2
#define GX_INDEX16      3

#define GX_POS_XY      0
#define GX_POS_XYZ      1
#define GX_NRM_XYZ      0
#define GX_NRM_NBT      1
#define GX_NRM_NBT3      2
#define GX_CLR_RGB      0
#define GX_CLR_RGBA      1
#define GX_TEX_S      0
#define GX_TEX_ST      1

#define GX_U8        0
#define GX_S8        1
#define GX_U16        2
#define GX_S16        3
#define GX_F32        4
#define GX_RGB565      0
#define GX_RGB8        1
#define GX_RGBX8      2
#define GX_RGBA4      3
#define GX_RGBA6      4
#define GX_RGBA8      5

#define  GX_CULL_ALL   3
#define  GX_CULL_BACK   2
#define  GX_CULL_FRONT   1
#define  GX_CULL_NONE   0

#define GX_TEXMAP0        0
#define GX_TEXMAP1        1
#define GX_TEXMAP2        2
#define GX_TEXMAP3        3
#define GX_TEXMAP4        4
#define GX_TEXMAP5        5
#define GX_TEXMAP6        6
#define GX_TEXMAP7        7
#define GX_MAX_TEXMAP      8
#define GX_TEXMAP_NULL      0xff
#define GX_TEXMAP_DISABLE    0x100

#define GX_TEVSTAGE0    0
#define GX_TEVSTAGE1    1
#define GX_TEVSTAGE2    2
#define GX_TEVSTAGE3    3
#define GX_TEVSTAGE4    4
#define GX_TEVSTAGE5    5
#define GX_TEVSTAGE6    6
#define GX_TEVSTAGE7    7
#define GX_TEVSTAGE8    8
#define GX_TEVSTAGE9    9
#define GX_TEVSTAGE10    10
#define GX_TEVSTAGE11    11
#define GX_TEVSTAGE12    12
#define GX_TEVSTAGE13    13
#define GX_TEVSTAGE14    14
#define GX_TEVSTAGE15    15
#define GX_MAX_TEVSTAGE    16

#define GX_TEXCOORD0    0x0
#define GX_TEXCOORD1    0x1
#define GX_TEXCOORD2    0x2
#define GX_TEXCOORD3    0x3
#define GX_TEXCOORD4    0x4
#define GX_TEXCOORD5    0x5
#define GX_TEXCOORD6    0x6
#define GX_TEXCOORD7    0x7
#define GX_MAXCOORD      0x8
#define GX_TEXCOORDNULL    0xff

#define GX_COLOR0      0
#define GX_COLOR1      1
#define GX_ALPHA0      2
#define GX_ALPHA1      3
#define GX_COLOR0A0      4
#define GX_COLOR1A1      5
#define GX_COLORZERO    6
#define GX_ALPHA_BUMP    7
#define GX_ALPHA_BUMPN    8
#define GX_COLORNULL    0xff

#define GX_MODULATE      0
#define GX_DECAL      1
#define GX_BLEND      2
#define GX_REPLACE      3
#define GX_PASSCLR      4

#define GX_CC_CPREV      0
#define GX_CC_APREV      1
#define GX_CC_C0      2
#define GX_CC_A0      3
#define GX_CC_C1      4
#define GX_CC_A1      5
#define GX_CC_C2      6
#define GX_CC_A2      7
#define GX_CC_TEXC      8
#define GX_CC_TEXA      9
#define GX_CC_RASC      10
#define GX_CC_RASA      11
#define GX_CC_ONE      12
#define GX_CC_HALF      13
#define GX_CC_KONST      14
#define GX_CC_ZERO      15

#define GX_CA_APREV      0
#define GX_CA_A0      1
#define GX_CA_A1      2
#define GX_CA_A2      3
#define GX_CA_TEXA      4
#define GX_CA_RASA      5
#define GX_CA_KONST      6
#define GX_CA_ZERO      7

#define GX_TEV_ADD        0
#define GX_TEV_SUB        1
#define GX_TEV_COMP_R8_GT    8
#define GX_TEV_COMP_R8_EQ    9
#define GX_TEV_COMP_GR16_GT    10
#define GX_TEV_COMP_GR16_EQ    11
#define GX_TEV_COMP_BGR24_GT  12
#define GX_TEV_COMP_BGR24_EQ  13
#define GX_TEV_COMP_RGB8_GT    14
#define GX_TEV_COMP_RGB8_EQ    15
#define GX_TEV_COMP_A8_GT    GX_TEV_COMP_RGB8_GT
#define GX_TEV_COMP_A8_EQ    GX_TEV_COMP_RGB8_EQ

#define GX_TB_ZERO        0
#define GX_TB_ADDHALF      1
#define GX_TB_SUBHALF      2
#define GX_MAX_TEVBIAS      3

#define GX_CS_SCALE_1      0
#define GX_CS_SCALE_2      1
#define GX_CS_SCALE_4      2
#define GX_CS_DIVIDE_2      3
#define GX_MAX_TEVSCALE      4

#define GX_TEVPREV        0
#define GX_TEVREG0        1
#define GX_TEVREG1        2
#define GX_TEVREG2        3
#define GX_MAX_TEVREG      4

#define GX_NEAR              0
#define GX_LINEAR            1
#define GX_NEAR_MIP_NEAR        2
#define GX_LIN_MIP_NEAR          3
#define GX_NEAR_MIP_LIN          4
#define GX_LIN_MIP_LIN          5

#define GX_ANISO_1            0
#define GX_ANISO_2            1
#define GX_ANISO_4            2
#define GX_MAX_ANISOTROPY        3

//typedef struct _gx_texobj {
//    u32 val[8];
//} GXTexObj;

//typedef struct {
//    u32 attr;
//    u32 type;
//} GXVtxDesc;
//
//typedef union _wgpipe {
//    u8 U8;
//    s8 S8;
//    u16 U16;
//    s16 S16;
//    u32 U32;
//    s32 S32;
//    f32 F32;
//} WGPipe;

typedef enum _GXTevStageID {
    GXTevStage0 = GX_TEVSTAGE0,
    GXTevStage1 = GX_TEVSTAGE1,
    GXTevStage2 = GX_TEVSTAGE2,
    GXTevStage3 = GX_TEVSTAGE3,
    GXTevStage4 = GX_TEVSTAGE4,
    GXTevStage5 = GX_TEVSTAGE5,
    GXTevStage6 = GX_TEVSTAGE6,
    GXTevStage7 = GX_TEVSTAGE7,
    GXTevStage8 = GX_TEVSTAGE8,
    GXTevStage9 = GX_TEVSTAGE9,
    GXTevStage10 = GX_TEVSTAGE10,
    GXTevStage11 = GX_TEVSTAGE11,
    GXTevStage12 = GX_TEVSTAGE12,
    GXTevStage13 = GX_TEVSTAGE13,
    GXTevStage14 = GX_TEVSTAGE14,
    GXTevStage15 = GX_TEVSTAGE15
} _GXTevStageID;


typedef enum _GXTexCoordID {
    GXTexCoord0 = GX_TEXCOORD0,
    GXTexCoord1 = GX_TEXCOORD1,
    GXTexCoord2 = GX_TEXCOORD2,
    GXTexCoord3 = GX_TEXCOORD3,
    GXTexCoord4 = GX_TEXCOORD4,
    GXTexCoord5 = GX_TEXCOORD5,
    GXTexCoord6 = GX_TEXCOORD6,
    GXTexCoord7 = GX_TEXCOORD7,
    GXTexCoordNull = GX_TEXCOORDNULL
} _GXTexCoordID;


typedef enum _GXTexMapID {
    GXTexMap0 = GX_TEXMAP0,
    GXTexMap1 = GX_TEXMAP1,
    GXTexMap2 = GX_TEXMAP2,
    GXTexMap3 = GX_TEXMAP3,
    GXTexMap4 = GX_TEXMAP4,
    GXTexMap5 = GX_TEXMAP5,
    GXTexMap6 = GX_TEXMAP6,
    GXTexMap7 = GX_TEXMAP7,
    GXTexMapNull = GX_TEXMAP_NULL

} _GXTexMapID;


typedef enum _GXChannelID {

    GXChannelColor0 = GX_COLOR0,
    GXChannelColor1 = GX_COLOR1,
    GXChannelAlpha0 = GX_ALPHA0,
    GXChannelAlpha1 = GX_ALPHA1,
    GXChannelColor0A0 = GX_COLOR0A0,
    GXChannelColor1A1 = GX_COLOR1A1,
    GXChannelColorzero = GX_COLORZERO,
    GXChannelAlphaBump = GX_ALPHA_BUMP,
    GXChannelAlphaBumpn = GX_ALPHA_BUMPN,
    GXChannelColorNull = GX_COLORNULL
} _GXChannelID;

typedef enum _GXCompare {
    GxCompare_NEVER = GX_NEVER,
    GxCompare_LESS = GX_LESS,
    GxCompare_EQUAL = GX_EQUAL,
    GxCompare_LEQUAL = GX_LEQUAL,
    GxCompare_GREATER = GX_GREATER,
    GxCompare_NEQUAL = GX_NEQUAL,
    GxCompare_GEQUAL = GX_GEQUAL,
    GxCompare_ALWAYS = GX_ALWAYS
} _GXCompare;

typedef enum _GXTevColorArg {
    GxTevColorArg_CPREV = GX_CC_CPREV,
    GxTevColorArg_APREV = GX_CC_APREV,
    GxTevColorArg_C0 = GX_CC_C0,
    GxTevColorArg_A0 = GX_CC_A0,
    GxTevColorArg_C1 = GX_CC_C1,
    GxTevColorArg_A1 = GX_CC_A1,
    GxTevColorArg_C2 = GX_CC_C2,
    GxTevColorArg_A2 = GX_CC_A2,
    GxTevColorArg_TEXC = GX_CC_TEXC,
    GxTevColorArg_TEXA = GX_CC_TEXA,
    GxTevColorArg_RASC = GX_CC_RASC,
    GxTevColorArg_RASA = GX_CC_RASA,
    GxTevColorArg_ONE = GX_CC_ONE,
    GxTevColorArg_HALF = GX_CC_HALF,
    GxTevColorArg_KONST = GX_CC_KONST,
    GxTevColorArg_ZERO = GX_CC_ZERO,
} _GXTevColorArg;

typedef enum _GXTevAlphaArg {
    GxTevAlphaArg_APREV = GX_CA_APREV,
    GxTevAlphaArg_A0 = GX_CA_A0,
    GxTevAlphaArg_A1 = GX_CA_A1,
    GxTevAlphaArg_A2 = GX_CA_A2,
    GxTevAlphaArg_TEXA = GX_CA_TEXA,
    GxTevAlphaArg_RASA = GX_CA_RASA,
    GxTevAlphaArg_KONST = GX_CA_KONST,
    GxTevAlphaArg_ZERO = GX_CA_ZERO,
} _GXTevAlphaArg;

typedef enum _GXTevOp {
    GxTevOp_ADD = GX_TEV_ADD,
    GxTevOp_SUB = GX_TEV_SUB,
    GxTevOp_COMP_R8_GT = GX_TEV_COMP_R8_GT,
    GxTevOp_COMP_R8_EQ = GX_TEV_COMP_R8_EQ,
    GxTevOp_COMP_GR16_GT = GX_TEV_COMP_GR16_GT,
    GxTevOp_COMP_GR16_EQ = GX_TEV_COMP_GR16_EQ,
    GxTevOp_COMP_BGR24_GT = GX_TEV_COMP_BGR24_GT,
    GxTevOp_COMP_BGR24_EQ = GX_TEV_COMP_BGR24_EQ,
    GxTevOp_COMP_RGB8_GT = GX_TEV_COMP_RGB8_GT,
    GxTevOp_COMP_RGB8_EQ = GX_TEV_COMP_RGB8_EQ,
    GxTevOp_COMP_A8_GT = GX_TEV_COMP_A8_GT,
    GxTevOp_COMP_A8_EQ = GX_TEV_COMP_A8_EQ,
} _GXTevOp;

typedef enum _GXTevBias {
    GxTevBias_ZERO = GX_TB_ZERO,
    GxTevBias_ADDHALF = GX_TB_ADDHALF,
    GxTevBias_SUBHALF = GX_TB_SUBHALF
} _GXTevBias;

typedef enum _GXTevScale {
    GxTevScale_SCALE_1 = GX_CS_SCALE_1,
    GxTevScale_SCALE_2 = GX_CS_SCALE_2,
    GxTevScale_SCALE_4 = GX_CS_SCALE_4,
    GxTevScale_DIVIDE_2 = GX_CS_DIVIDE_2,
} _GXTevScale;

typedef enum _GXTevRegID {
    GxTevRegID_TEVPREV = GX_TEVPREV,
    GxTevRegID_TEVREG0 = GX_TEVREG0,
    GxTevRegID_TEVREG1 = GX_TEVREG1,
    GxTevRegID_TEVREG2 = GX_TEVREG2,
} _GXTevRegID;

typedef enum _GXBlendMode {
    GxBlendMode_NONE = GX_BM_NONE,
    GxBlendMode_BLEND = GX_BM_BLEND,
    GxBlendMode_LOGIC = GX_BM_LOGIC,
    GxBlendMode_SUBTRACT = GX_BM_SUBTRACT,
} _GXBlendMode;

typedef enum _GXBlendFactor {
    GxBlendFactor_ZERO = GX_BL_ZERO,
    GxBlendFactor_ONE = GX_BL_ONE,
    GxBlendFactor_SRCCLR = GX_BL_SRCCLR,
    GxBlendFactor_INVSRCCLR = GX_BL_INVSRCCLR,
    GxBlendFactor_SRCALPHA = GX_BL_SRCALPHA,
    GxBlendFactor_INVSRCALPHA = GX_BL_INVSRCALPHA,
    GxBlendFactor_DSTALPHA = GX_BL_DSTALPHA,
    GxBlendFactor_INVDSTALPHA = GX_BL_INVDSTALPHA,
    GxBlendFactor_DSTCLR = GX_BL_DSTCLR,
    GxBlendFactor_INVDSTCLR = GX_BL_INVDSTCLR,
} _GXBlendFactor;

typedef enum _GXLogicOp {
    GxLogicOp_CLEAR = GX_LO_CLEAR,
    GxLogicOp_AND = GX_LO_AND,
    GxLogicOp_REVAND = GX_LO_REVAND,
    GxLogicOp_COPY = GX_LO_COPY,
    GxLogicOp_INVAND = GX_LO_INVAND,
    GxLogicOp_NOOP = GX_LO_NOOP,
    GxLogicOp_XOR = GX_LO_XOR,
    GxLogicOp_OR = GX_LO_OR,
    GxLogicOp_NOR = GX_LO_NOR,
    GxLogicOp_EQUIV = GX_LO_EQUIV,
    GxLogicOp_INV = GX_LO_INV,
    GxLogicOp_REVOR = GX_LO_REVOR,
    GxLogicOp_INVCOPY = GX_LO_INVCOPY,
    GxLogicOp_INVOR = GX_LO_INVOR,
    GxLogicOp_NAND = GX_LO_NAND,
    GxLogicOp_SET = GX_LO_SET,
} _GXLogicOp;

extern "C" {

void GXInitTexObj(GXTexObj *obj, void *img_ptr,
                  u16 wd, u16 ht,
                  u8 fmt,
                  u8 wrap_s, u8 wrap_t, u8 mipmap
);

void GXLoadTexObj(GXTexObj *obj, u8 mapid);

void GXSetZMode(u8 enable, u8 func, u8 update_enable);
void GXSetBlendMode(u8 type, u8 src_fact, u8 dst_fact, u8 op);
void GXBegin(u8 primitve, u8 vtxfmt, u16 vtxcnt);
void GXInvalidateVtxCache();
void GXInvalidateTexAll();
void GXClearVtxDesc();
void GXSetVtxDesc(u8 attr, u8 type);
void GXSetVtxDescv(GXVtxDesc *attr_list);
void GXSetVtxAttrFmt(u8 vtxfmt, u32 vtxattr, u32 comptype, u32 compsize, u32 frac);
void GXSetCullMode(u8 mode);
void GXDrawDone();

void GXSetNumChans(u8 num);
void GXSetNumTexGens(u32 nr);

void GXSetNumTevStages(u8 num);
void GXSetTevOrder(u8 tevstage, u8 texcoord, u32 texmap, u8 color);
void GXSetTevOp(u8 tevstage, u8 mode);
void GXInitTexObjLOD(GXTexObj *obj,
                     u8 minfilt, u8 magfilt,
                     f32 minlod, f32 maxlod,
                     f32 lodbias, u8 biasclamp,
                     u8 edgelod, u8 maxaniso);

// Push values
//extern WGPipe *const wgPipe;
#define wgPipe ((WGPipe*)0xCC008000)

//static inline void GX_Position3f32(f32 x, f32 y, f32 z) {
//  wgPipe->F32 = x;
//  wgPipe->F32 = y;
//  wgPipe->F32 = z;
//}
//
//static inline void GX_Position3u16(u16 x, u16 y, u16 z) {
//  wgPipe->U16 = x;
//  wgPipe->U16 = y;
//  wgPipe->U16 = z;
//}
//
//static inline void GX_Position3s16(s16 x, s16 y, s16 z) {
//  wgPipe->S16 = x;
//  wgPipe->S16 = y;
//  wgPipe->S16 = z;
//}
//
//static inline void GX_Position3u8(u8 x, u8 y, u8 z) {
//  wgPipe->U8 = x;
//  wgPipe->U8 = y;
//  wgPipe->U8 = z;
//}
//
//static inline void GX_Position3s8(s8 x, s8 y, s8 z) {
//  wgPipe->S8 = x;
//  wgPipe->S8 = y;
//  wgPipe->S8 = z;
//}
//
//static inline void GX_Position2f32(f32 x, f32 y) {
//  wgPipe->F32 = x;
//  wgPipe->F32 = y;
//}
//
//static inline void GX_Position2u16(u16 x, u16 y) {
//  wgPipe->U16 = x;
//  wgPipe->U16 = y;
//}
//
//static inline void GX_Position2s16(s16 x, s16 y) {
//  wgPipe->S16 = x;
//  wgPipe->S16 = y;
//}
//
//static inline void GX_Position2u8(u8 x, u8 y) {
//  wgPipe->U8 = x;
//  wgPipe->U8 = y;
//}
//
//static inline void GX_Position2s8(s8 x, s8 y) {
//  wgPipe->S8 = x;
//  wgPipe->S8 = y;
//}
//
//static inline void GX_Position1x8(u8 index) {
//  wgPipe->U8 = index;
//}
//
//static inline void GX_Position1x16(u16 index) {
//  wgPipe->U16 = index;
//}
//
//static inline void GX_Normal3f32(f32 nx, f32 ny, f32 nz) {
//  wgPipe->F32 = nx;
//  wgPipe->F32 = ny;
//  wgPipe->F32 = nz;
//}
//
//static inline void GX_Normal3s16(s16 nx, s16 ny, s16 nz) {
//  wgPipe->S16 = nx;
//  wgPipe->S16 = ny;
//  wgPipe->S16 = nz;
//}
//
//static inline void GX_Normal3s8(s8 nx, s8 ny, s8 nz) {
//  wgPipe->S8 = nx;
//  wgPipe->S8 = ny;
//  wgPipe->S8 = nz;
//}
//
//static inline void GX_Normal1x8(u8 index) {
//  wgPipe->U8 = index;
//}
//
//static inline void GX_Normal1x16(u16 index) {
//  wgPipe->U16 = index;
//}
//
//static inline void GX_Color4u8(u8 r, u8 g, u8 b, u8 a) {
//  wgPipe->U8 = r;
//  wgPipe->U8 = g;
//  wgPipe->U8 = b;
//  wgPipe->U8 = a;
//}
//
//static inline void GX_Color3u8(u8 r, u8 g, u8 b) {
//  wgPipe->U8 = r;
//  wgPipe->U8 = g;
//  wgPipe->U8 = b;
//}
//
//static inline void GX_Color3f32(f32 r, f32 g, f32 b) {
//  wgPipe->U8 = (u8) (r * 255.0);
//  wgPipe->U8 = (u8) (g * 255.0);
//  wgPipe->U8 = (u8) (b * 255.0);
//}
//
//static inline void GX_Color1u32(u32 clr) {
//  wgPipe->U32 = clr;
//}
//
//static inline void GX_Color1u16(u16 clr) {
//  wgPipe->U16 = clr;
//}
//
//static inline void GX_Color1x8(u8 index) {
//  wgPipe->U8 = index;
//}
//
//static inline void GX_Color1x16(u16 index) {
//  wgPipe->U16 = index;
//}
//
//static inline void GX_TexCoord2f32(f32 s, f32 t) {
//  wgPipe->F32 = s;
//  wgPipe->F32 = t;
//}
//
//static inline void GX_TexCoord2u16(u16 s, u16 t) {
//  wgPipe->U16 = s;
//  wgPipe->U16 = t;
//}
//
//static inline void GX_TexCoord2s16(s16 s, s16 t) {
//  wgPipe->S16 = s;
//  wgPipe->S16 = t;
//}
//
//static inline void GX_TexCoord2u8(u8 s, u8 t) {
//  wgPipe->U8 = s;
//  wgPipe->U8 = t;
//}
//
//static inline void GX_TexCoord2s8(s8 s, s8 t) {
//  wgPipe->S8 = s;
//  wgPipe->S8 = t;
//}
//
//static inline void GX_TexCoord1f32(f32 s) {
//  wgPipe->F32 = s;
//}
//
//static inline void GX_TexCoord1u16(u16 s) {
//  wgPipe->U16 = s;
//}
//
//static inline void GX_TexCoord1s16(s16 s) {
//  wgPipe->S16 = s;
//}
//
//static inline void GX_TexCoord1u8(u8 s) {
//  wgPipe->U8 = s;
//}
//
//static inline void GX_TexCoord1s8(s8 s) {
//  wgPipe->S8 = s;
//}
//
//static inline void GX_TexCoord1x8(u8 index) {
//  wgPipe->U8 = index;
//}
//
//static inline void GX_TexCoord1x16(u16 index) {
//  wgPipe->U16 = index;
//}
//
//static inline void GX_MatrixIndex1x8(u8 index) {
//  wgPipe->U8 = index;
//}

}

#endif //PRIME_PRACTICE_GX_HPP
