#include "ImGuiEngine.hpp"
#include "font_atlas.hpp"
#include "imgui.h"
#include "malloc_wrappers.h"
#include "os.h"
#include "prime/CPlayerState.hpp"
#include "prime/CScriptDock.hpp"
#include "prime/CWorld.hpp"
#include <imgui_internal.h>

GXTexObj imguiFontTexture;
IMGuiGXTexture imuiFontTextureImguiTexture = {};

const ImWchar EMPTY_FONT_RANGE[] = {
    0 // end
};

float char_to_float(int width, unsigned char c) {
  return ((float) c / (float) width);
}

#define PI 3.14159265

void ImGuiEngine::ImGui_Render(const ImDrawData *drawData) {
#ifdef USE_GX
  ImGui_Render_GX(drawData);
#else
  ImGui_Render_Stream(drawData);
#endif
}

#ifdef USE_GX
void ImGuiEngine::ImGui_Render_GX(const ImDrawData *drawData) {
  // Setup for GX
  // Prime's formats, which are what I want so that's convenient:
  // 0 9 1 4 0 GX_VTXFMT0 GX_VA_POS   GX_POS_XYZ  GX_F32    0
  // 0 a 0 4 0 GX_VTXFMT0 GX_VA_NRM   GX_NRM_XYZ  GX_F32    0
  // 0 b 1 5 0 GX_VTXFMT0 GX_VA_CLR0  GX_CLR_RGBA GX_RGBA8  0
  // 0 d 1 4 0 GX_VTXFMT0 GX_VA_TEX0  GX_TEX_ST   GX_F32    0
  // Repeat for all GX_VA_TEXs

  CGraphics::DisableAllLights();
  CGX::SetZMode(false, GxCompare_NEVER, false);
  //  GXSetBlendMode(
  //      GX_BM_BLEND,
  //      GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
  //      GX_LO_NOOP
  //  );
  CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
  //  CGraphics::SetAlphaCompare(ERglAlphaFunc_GREATER, 0, ERglAlphaOp_OR, ERglAlphaFunc_GREATER, 0);
  CGraphics::SetCullMode(ERglCullMode_None);
  CTexture::InvalidateTexmap(GX_TEXMAP0);

  CGX::SetNumTevStages(1);
  CGX::SetTevOrder(
      GXTevStage0,
      GXTexCoord0,
      GXTexMap0,
      GXChannelColor0A0
  );
  CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_TEXC, GxTevColorArg_RASC, GxTevColorArg_ZERO);
  CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_TEXA, GxTevAlphaArg_RASA, GxTevAlphaArg_ZERO);
  CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
  CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);

  float left = drawData->DisplayPos.x;
  float right = left + drawData->DisplaySize.x;
  float top = drawData->DisplayPos.y;
  float bottom = top + drawData->DisplaySize.y;
  CGraphics::SetOrtho(left, right, top, bottom, -1, 1);
  CGraphics::SetIdentityModelMatrix();
  CGraphics::SetIdentityViewPointMatrix();
  // Have to rotate the model matrix by 90 degrees along the X axis so it's XY instead of XZ
  CTransform4f vp{};
  memset(&vp, 0, sizeof(CTransform4f));
  float theta = PI / 2.0;
  vp.mtx[0][0] = 1;
  vp.mtx[1][1] = CMath::FastCosR(theta);
  vp.mtx[1][2] = -CMath::FastSinR(theta);
  vp.mtx[2][1] = CMath::FastSinR(theta);
  vp.mtx[2][2] = CMath::FastCosR(theta);
  CGraphics::SetModelMatrix(vp);

  //Set up vert format
  GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
  GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
  GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
  static _GXVtxDescList desc[4] = {
      {.attr=GX_VA_POS, .type=GX_INDEX16},
      {.attr=GX_VA_CLR0, .type=GX_INDEX16},
      {.attr=GX_VA_TEX0, .type=GX_INDEX16},
      {.attr=GX_VA_NULL, .type=GX_NONE}
  };
  CGX::SetVtxDescv(desc);
  CGraphics::SetTevStates(6); // tex | color
  CGX::SetNumTexGens(2); // Apparently required
  WGPipe *const pipe = (WGPipe *) 0xCC008000;
  for (int cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; cmdListIdx++) {
    const ImDrawList *cmdList = drawData->CmdLists[cmdListIdx];
    // set up array
    const char *vtxBuffer = reinterpret_cast<const char *>(cmdList->VtxBuffer.Data);
    // Make sure it's flushed to memory so the GPU can read it
    DCFlushRange(cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_POS, vtxBuffer + offsetof(ImDrawVert, pos), sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_CLR0, vtxBuffer + offsetof(ImDrawVert, col), sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_TEX0, vtxBuffer + offsetof(ImDrawVert, uv), sizeof(ImDrawVert));
    // For each cmdlist
    for (int cmdBufferIdx = 0; cmdBufferIdx < cmdList->CmdBuffer.Size; cmdBufferIdx++) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdBufferIdx];
      IMGuiGXTexture *tex = (IMGuiGXTexture *) cmd->TextureId;
      GXLoadTexObj(tex->obj, GX_TEXMAP0);
      if (tex->tlut) {
        GXLoadTlut(tex->tlut, tex->tlut_name);
      }

      // (x0, y0, x1, y1) but also need to flip the two y coords
      int x0 = (int) cmd->ClipRect.x;
      int x1 = (int) cmd->ClipRect.z;
      int y0 = (int) (bottom - cmd->ClipRect.w);
      int y1 = (int) (bottom - cmd->ClipRect.y);
      int clipX = x0;
      int clipY = y0;
      int clipW = x1 - x0;
      int clipH = y1 - y0;
      CGraphics::SetScissor(clipX, clipY, clipW, clipH);
      IM_ASSERT(cmd->ElemCount <= 65535);
      CGX::Begin(GX_TRIANGLES, GX_VTXFMT0, cmd->ElemCount);
      for (unsigned int elemIdx = 0; elemIdx < cmd->ElemCount; elemIdx++) {
        const ImDrawIdx idx = cmdList->IdxBuffer[cmd->IdxOffset + elemIdx];
        // the inline methods don't work for some reason. Optimizer gets a bit handsy.
        pipe->U16 = idx; // pos
        pipe->U16 = idx; // color
        pipe->U16 = idx; // tex
      }
      CGX::End();
    }
  } // done with rendering
  CGraphics::SetScissor((int) left, (int) top, (int) right, (int) bottom);
  CGraphics::SetDefaultVtxAttrFmt();
}
#else
void ImGuiEngine::ImGui_Render_Stream(const ImDrawData *drawData) {
  // Setup for GX
  // Prime's formats, which are what I want so that's convenient:
  // 0 9 1 4 0 GX_VTXFMT0 GX_VA_POS   GX_POS_XYZ  GX_F32    0
  // 0 a 0 4 0 GX_VTXFMT0 GX_VA_NRM   GX_NRM_XYZ  GX_F32    0
  // 0 b 1 5 0 GX_VTXFMT0 GX_VA_CLR0  GX_CLR_RGBA GX_RGBA8  0
  // 0 d 1 4 0 GX_VTXFMT0 GX_VA_TEX0  GX_TEX_ST   GX_F32    0
  // Repeat for all GX_VA_TEXs

  CGraphics::DisableAllLights();
  CGX::SetZMode(false, GxCompare_NEVER, false);
  //  GXSetBlendMode(
  //      GX_BM_BLEND,
  //      GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
  //      GX_LO_NOOP
  //  );
  CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
  //  CGraphics::SetAlphaCompare(ERglAlphaFunc_GREATER, 0, ERglAlphaOp_OR, ERglAlphaFunc_GREATER, 0);
  CGraphics::SetCullMode(ERglCullMode_None);
  GXLoadTexObj(&imguiFontTexture, GX_TEXMAP0);
  CTexture::InvalidateTexmap(GX_TEXMAP0);

  CGX::SetNumTevStages(1);
  CGX::SetTevOrder(
      GXTevStage0,
      GXTexCoord0,
      GXTexMap0,
      GXChannelColor0A0
  );
  CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_TEXC, GxTevColorArg_RASC, GxTevColorArg_ZERO);
  CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_TEXA, GxTevAlphaArg_RASA, GxTevAlphaArg_ZERO);
  CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
  CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);

  float left = drawData->DisplayPos.x;
  float right = left + drawData->DisplaySize.x;
  float top = drawData->DisplayPos.y;
  float bottom = top + drawData->DisplaySize.y;
  CGraphics::SetOrtho(left, right, top, bottom, -1, 1);
  CGraphics::SetIdentityModelMatrix();
  CGraphics::SetIdentityViewPointMatrix();
  // Have to rotate the model matrix by 90 degrees along the X axis so it's XY instead of XZ
  CTransform4f vp{};
  memset(&vp, 0, sizeof(CTransform4f));
  float theta = PI / 2.0;
  vp.mtx[0][0] = 1;
  vp.mtx[1][1] = CMath::FastCosR(theta);
  vp.mtx[1][2] = -CMath::FastSinR(theta);
  vp.mtx[2][1] = CMath::FastSinR(theta);
  vp.mtx[2][2] = CMath::FastCosR(theta);
  CGraphics::SetModelMatrix(vp);

  constexpr int maxIdxPerBatch = 3 * 30;
  int idxPerBatch = 0;
  for (int cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; cmdListIdx++) {
    const ImDrawList *cmdList = drawData->CmdLists[cmdListIdx];
    // set up array
    const char *vtxBuffer = reinterpret_cast<const char *>(cmdList->VtxBuffer.Data);
    // Make sure it's flushed to memory so the GPU can read it
    DCFlushRange(cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_POS, vtxBuffer + offsetof(ImDrawVert, pos), sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_CLR0, vtxBuffer + offsetof(ImDrawVert, col), sizeof(ImDrawVert));
    CGX::SetArray(GX_VA_TEX0, vtxBuffer + offsetof(ImDrawVert, uv), sizeof(ImDrawVert));
    // For each cmdlist
    for (int cmdBufferIdx = 0; cmdBufferIdx < cmdList->CmdBuffer.Size; cmdBufferIdx++) {
      const ImDrawCmd *cmd = &cmdList->CmdBuffer[cmdBufferIdx];
      // (x0, y0, x1, y1) but also need to flip the two y coords
      int x0 = (int) cmd->ClipRect.x;
      int x1 = (int) cmd->ClipRect.z;
      int y0 = (int) (bottom - cmd->ClipRect.w);
      int y1 = (int) (bottom - cmd->ClipRect.y);
      int clipX = x0;
      int clipY = y0;
      int clipW = x1 - x0;
      int clipH = y1 - y0;
      CGraphics::SetScissor(clipX, clipY, clipW, clipH);
      CGraphics::StreamBegin(ERglPrimitive_TRIANGLES);
      for (int elemIdx = 0; elemIdx < cmd->ElemCount; elemIdx++) {
        const ImDrawIdx *idx = &cmdList->IdxBuffer[cmd->IdxOffset + elemIdx];
        const ImDrawVert *dataStart = &cmdList->VtxBuffer[cmd->VtxOffset + *idx];

        CGraphics::StreamColor(dataStart->col);
        CGraphics::StreamTexcoord(dataStart->uv.x, dataStart->uv.y);
        CGraphics::StreamVertex(dataStart->pos.x, dataStart->pos.y, 0);
        idxPerBatch++;
        if (idxPerBatch > maxIdxPerBatch && idxPerBatch % 3 == 0) {
          idxPerBatch = 0;
          CGraphics::FlushStream();
          CGraphics::ResetVertexDataStream(false);
        }
      }
      idxPerBatch = 0;
      CGraphics::StreamEnd();
    }
  } // done with rendering
  CGraphics::SetScissor((int) left, (int) top, (int) right, (int) bottom);
  CGraphics::SetDefaultVtxAttrFmt();
}
#endif

void ImGuiEngine::ImGui_Init() {
  // see https://github.com/MetroidPrimeModding/imgui-font-atlas-generator
  // for how this stuff was generated

  // init
  ImGui::SetAllocatorFunctions(
      &prime_malloc, &prime_free
  );

  OSReport("Create context \n");
  ImFontAtlas *atlas = IM_NEW(ImFontAtlas);
  atlas->TexDesiredWidth = 512;
  atlas->Flags |= ImFontAtlasFlags_NoMouseCursors;
  atlas->TexWidth = FontAtlas::ATLAS_W;
  atlas->TexHeight = FontAtlas::ATLAS_H;
  atlas->TexUvScale = ImVec2(1.f / (float) FontAtlas::ATLAS_W, 1.f / (float) FontAtlas::ATLAS_H);
  atlas->TexUvWhitePixel = FontAtlas::WhitePixel;
  atlas->PackIdLines = FontAtlas::PackIdMouseCursors;
  atlas->PackIdMouseCursors = 0;
  for (int i = 0; i < IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1; i++) {
    auto uvline = &FontAtlas::TexUvLines[i];
    atlas->TexUvLines[i] = ImVec4{
        char_to_float(FontAtlas::ATLAS_W, uvline->W),
        char_to_float(FontAtlas::ATLAS_H, uvline->X),
        char_to_float(FontAtlas::ATLAS_W, uvline->Y),
        char_to_float(FontAtlas::ATLAS_H, uvline->Z),
    };
  }
  ImGui::CreateContext(atlas);

  ImGui::StyleColorsDark();
  ImGui::GetStyle().AntiAliasedLines = false;
  ImGui::GetStyle().AntiAliasedLinesUseTex = false;
  ImGui::GetStyle().AntiAliasedFill = false;

  // Setup basic flags
  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "gx";
  io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

//  // setup font
  {
    ImFontConfig fontConfig{};
    fontConfig.SizePixels = 10;
    fontConfig.OversampleH = 1;
    fontConfig.OversampleV = 1;
    fontConfig.PixelSnapH = true;
    fontConfig.GlyphRanges = io.Fonts->GetGlyphRangesNone();
    // gen font and font data
    io.Fonts->AddFontBlank(&fontConfig);
  }
  for (auto &v : FontAtlas::CustomRects) {
    ImFontAtlasCustomRect rect{};
    rect.X = v.y;
    rect.Y = v.z;
    rect.Height = v.w;
    rect.Width = v.x;
    atlas->CustomRects.push_back(rect);
  }
  ImFont *font = io.Fonts->Fonts[0];
  for (float f : FontAtlas::INDEX_ADVANCE_X) {
    font->IndexAdvanceX.push_back(f);
  }
  font->FallbackAdvanceX = FontAtlas::FALLBACK_ADVANCE_X;
  font->FontSize = FontAtlas::FONT_SIZE;
  for (auto v : FontAtlas::INDEX_LOOKUP) {
    font->IndexLookup.push_back(v);
  }
  for (auto v : FontAtlas::FONT_GLYPHS) {
    font->Glyphs.push_back(ImFontGlyph{
        .Colored=v.Colored,
        .Visible=v.Visible,
        .Codepoint=v.Codepoint,
        .AdvanceX=(float) v.AdvanceX,
        .X0=(float) v.X0, .Y0=(float) v.Y0,
        .X1=(float) v.X1, .Y1=(float) v.Y1,
        .U0=char_to_float(FontAtlas::ATLAS_W, v.U0), .V0=char_to_float(FontAtlas::ATLAS_H, v.V0),
        .U1=char_to_float(FontAtlas::ATLAS_W, v.U1), .V1= char_to_float(FontAtlas::ATLAS_H, v.V1)
    });
  }
  font->Ascent = FontAtlas::ASCENT;
  font->Descent = FontAtlas::DESCENT;
  font->MetricsTotalSurface = FontAtlas::MetricsTotalSurface;
  for (int i = 0; i < sizeof(FontAtlas::Used4kPagesMap); i++) {
    font->Used4kPagesMap[i] = FontAtlas::Used4kPagesMap[i];
  }
  font->ContainerAtlas = io.Fonts;
  font->EllipsisChar = -1;
  imuiFontTextureImguiTexture.obj = &imguiFontTexture;
  io.Fonts->SetTexID(&imuiFontTextureImguiTexture);
  // send it off to GX
  GXInitTexObj(&imguiFontTexture, (void *) FontAtlas::ATLAS_DATA,
               FontAtlas::ATLAS_W, FontAtlas::ATLAS_H,
               GX_TF_I4,
               GX_CLAMP, GX_CLAMP,
               GX_FALSE
  );
  GXInitTexObjLOD(&imguiFontTexture,
                  GX_NEAR, GX_NEAR,
                  0, 0,
                  0,
                  GX_DISABLE,
                  GX_DISABLE,
                  GX_ANISO_1
  );
}

void ImGuiEngine::ImGui_Init_Style() {
  ImGuiStyle *style = &ImGui::GetStyle();
  style->WindowBorderSize = 0;
  style->WindowPadding = ImVec2(2, 2);
  style->WindowMinSize = ImVec2(2, 2);
}
