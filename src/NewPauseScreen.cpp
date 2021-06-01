#include <prime/CScriptDoor.hpp>
#include <prime/CScriptCameraHint.hpp>
#include <UI/PlayerMenu.hpp>
#include <UI/InventoryMenu.hpp>
#include <UI/MonitorWindow.hpp>
#include <UI/SettingsMenu.hpp>
#include "os.h"
#include "NewPauseScreen.hpp"
#include "prime/CGameState.hpp"
#include "prime/CWorldState.hpp"
#include "prime/CScriptRelay.hpp"
#include "prime/CScriptDock.hpp"
#include "prime/CGameGlobalObjects.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CWorld.hpp"
#include "prime/CMain.hpp"
#include "prime/CSfxManager.hpp"
#include "imgui.h"
#include "duk_mem.h"
#include "UI/WarpMenu.h"
#include "settings.hpp"
#include "font_atlas.hpp"

#define PAD_MAX_CONTROLLERS 4

GXTexObj imguiFontTexture;

NewPauseScreen *NewPauseScreen::instance = NULL;

NewPauseScreen::NewPauseScreen() {
  OSReport("Hello, Dolphin\n");

  InitIMGui_BundledFont();
  this->hide();
  inputs = new CFinalInput[4];

  // Patch CScriptTrigger so we can attach a value to it
  // CScriptTrigger::CScriptTrigger
  // 50A03E30 rlwimi r0, r5, 7, 24, 24
  // 98140148 stb r0, 0x148(r20)
  // lbz r0, 0x148(r20) 0x88140148

  *((u32 *) 0x80076f0c) = 0x90940148; // stw r4, 0x148(r20)
  *((u32 *) 0x80076f10) = 0x50A03E30; // rlwimi r0, r5, 7, 24, 24
  *((u32 *) 0x80076f14) = 0x98140148; // stb r0, 0x148(r20)

  // Patch file select IGT to mm:ss
  // r5 contains number of seconds
  // we want 5 and 6 as out, so

  // r7 = 60
  // r7 = r5 / r7 (seconds / 60) = minutes
  // r6 = r7 * 60 (minutes * 60)
  // r6 = r5 - r6 (seconds - (minutes * 60))
  // r5 = r7 // minutes
  /*
  li 7, 60
  divw 7, 5, 7
  mulli 6, 7, 60
  sub 6, 5, 6
  mr 5, 7
   */

  // nop out our region and overwrite
  for (u32 i = 0x8001FEF4; i <= 0x8001FF38; i += 4) {
    *((u32 *) i) = 0x60000000; // nop
  }
  *((u32 *) 0x8001FEF4) = 0x38e0003C; // li r7, 60
  *((u32 *) 0x8001FEF8) = 0x7CE53BD6; // divw r7, r5, r7
  *((u32 *) 0x8001FEFC) = 0x1CC7003C; // muli r6, r7, 60
  *((u32 *) 0x8001FF00) = 0x7CC62850; // sub r6, r5, r6
  *((u32 *) 0x8001FF04) = 0x7CE53B78; // mr r5, r7

  // Swap what text is used for ELAPSED to blank
  *((u32 *) 0x8001FFB8) = 0x3880005C; // li r4, 92 - which is blank
}

void NewPauseScreen::Render() {
  if (pauseScreenActive) {
    frames++;
  }
  NewPauseScreen::RenderMenu();
}

void NewPauseScreen::hide() {
  pauseScreenActive = false;
  frames = -1;
}

void NewPauseScreen::show() {
  this->pauseScreenActive = true;
  this->frames = 0;
}

void NewPauseScreen::HandleInputs() {
  if (this->pauseScreenActive) {
    ImGuiIO *io = &ImGui::GetIO();
    io->NavInputs[ImGuiNavInput_Activate] = inputs[0].DA();
    io->NavInputs[ImGuiNavInput_Cancel] = inputs[0].DB();
//    io->NavInputs[ImGuiNavInput_Menu] = inputs[0].DX();
//    io->NavInputs[ImGuiNavInput_Input] = inputs[0].DY();

    // dpad
    io->NavInputs[ImGuiNavInput_DpadLeft] = inputs[0].DDPLeft() || inputs[0].DLALeft();
    io->NavInputs[ImGuiNavInput_DpadRight] = inputs[0].DDPRight() || inputs[0].DLARight();
    io->NavInputs[ImGuiNavInput_DpadUp] = inputs[0].DDPUp() || inputs[0].DLAUp();
    io->NavInputs[ImGuiNavInput_DpadDown] = inputs[0].DDPDown() || inputs[0].DLADown();
    // analog
    io->NavInputs[ImGuiNavInput_LStickLeft] = inputs[0].ARALeft();
    io->NavInputs[ImGuiNavInput_LStickRight] = inputs[0].ARARight();
    io->NavInputs[ImGuiNavInput_LStickUp] = inputs[0].ARAUp();
    io->NavInputs[ImGuiNavInput_LStickDown] = inputs[0].ARADown();


    /*
     *     MAP_BUTTON(ImGuiNavInput_Activate,      SDL_CONTROLLER_BUTTON_A);               // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel,        SDL_CONTROLLER_BUTTON_B);               // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu,          SDL_CONTROLLER_BUTTON_X);               // Square / X
    MAP_BUTTON(ImGuiNavInput_Input,         SDL_CONTROLLER_BUTTON_Y);               // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft,      SDL_CONTROLLER_BUTTON_DPAD_LEFT);       // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight,     SDL_CONTROLLER_BUTTON_DPAD_RIGHT);      // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp,        SDL_CONTROLLER_BUTTON_DPAD_UP);         // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown,      SDL_CONTROLLER_BUTTON_DPAD_DOWN);       // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft,    SDL_CONTROLLER_AXIS_LEFTX, -thumb_dead_zone, -32768);
    MAP_ANALOG(ImGuiNavInput_LStickRight,   SDL_CONTROLLER_AXIS_LEFTX, +thumb_dead_zone, +32767);
    MAP_ANALOG(ImGuiNavInput_LStickUp,      SDL_CONTROLLER_AXIS_LEFTY, -thumb_dead_zone, -32767);
    MAP_ANALOG(ImGuiNavInput_LStickDown,    SDL_CONTROLLER_AXIS_LEFTY, +thumb_dead_zone, +32767);
     */
  }
}

void warp(uint32_t world, uint32_t area) {
  CAssetId worldID = (CAssetId) (world);
  CAssetId areaID = (CAssetId) (area);

  CStateManager *mgr = ((CStateManager *) 0x8045A1A8);
  mgr->GetWorld()->SetPauseState(true);

  CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
  CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId);
  gameState->SetCurrentWorldId(worldID);
  gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);

  CMain *cmain = *((CMain **) 0x805A8C38);
  cmain->SetFlowState(EFlowState_None);

  mgr->SetShouldQuitGame(true);

  NewPauseScreen::instance->hide();

}

bool NewPauseScreen::shouldRenderGloballyInsteadOfInWorld() {
  // TODO: properly fix the in-game renderer? Or handle this better.
  // For now the garble is preferable so it renders always
//    return this->active;
  return true;
}

void NewPauseScreen::RenderMenu() {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(SVIEWPORT_GLOBAL->x8_width, SVIEWPORT_GLOBAL->xc_height);
  io.DeltaTime = 1.f / 60.f;

  ImGui::NewFrame();

  if (this->pauseScreenActive && this->menuActive) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_None, ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(0, 0),
        ImVec2(400, SVIEWPORT_GLOBAL->xc_height - 20)
    );
    ImGui::Begin(
        "Practice Mod", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoTitleBar
    );
    GUI::drawWarpMenu();
    GUI::drawPlayerMenu();
    GUI::drawInventoryMenu();
    GUI::drawSettingsMenu();
    ImGui::End();
  }

  GUI::drawMonitorWindow(inputs);
  ImDrawList *dl = ImGui::GetBackgroundDrawList();
//  for (float y = 0; y < ImGui::GetIO().DisplaySize.y/2.0f; y += 5.0f) {
//    dl->AddLine(
//        ImVec2(0, y),
//        ImVec2(ImGui::GetIO().DisplaySize.x/2.0f, y),
//        IM_COL32(255, 255, 255, 255),
//        4.0f
//    );
//  }

  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();

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

  // do this the dumb way first

  //*****the dumb way******//
  // This is the max characters that actually works per render, for some reason
  // 30 quads, or 154 verts, or 1386 floats, or 5544 bytes
  // Not sure why that's the limit...
  int maxIdxPerBatch = 3 * 30;
  int idxPerBatch = 0;

  CGraphics::StreamBegin(ERglPrimitive_TRIANGLES);
  for (int cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; cmdListIdx++) {
    const ImDrawList *cmdList = drawData->CmdLists[cmdListIdx];
    // For each cmdlist
    for (int cmdBufferIdx = 0; cmdBufferIdx < cmdList->CmdBuffer.Size; cmdBufferIdx++) {
      const ImDrawCmd *cmdBuffer = &cmdList->CmdBuffer[cmdBufferIdx];
      for (int elemIdx = 0; elemIdx < cmdBuffer->ElemCount; elemIdx++) {
        const ImDrawIdx *idx = &cmdList->IdxBuffer[cmdBuffer->IdxOffset + elemIdx];
        const ImDrawVert *dataStart = &cmdList->VtxBuffer[cmdBuffer->VtxOffset + *idx];

        float r = (float) ((dataStart->col >> 0) & 0xFF) / 255.0f;
        float g = (float) ((dataStart->col >> 8) & 0xFF) / 255.0f;
        float b = (float) ((dataStart->col >> 16) & 0xFF) / 255.0f;
        float a = (float) ((dataStart->col >> 24) & 0xFF) / 255.0f;
//        r = g = b = a = 1;
//        a = 1;
        CGraphics::StreamColor(r, g, b, a);
        CGraphics::StreamTexcoord(dataStart->uv.x, dataStart->uv.y);
        CGraphics::StreamVertex(dataStart->pos.x, 0, dataStart->pos.y);
        idxPerBatch++;
        if (idxPerBatch > maxIdxPerBatch && idxPerBatch % 3 == 0) {
          idxPerBatch = 0;
          CGraphics::FlushStream();
        }
      }
      // flsuh between buffers
      idxPerBatch = 0;
      CGraphics::FlushStream();
    }
  }
  CGraphics::StreamEnd();
}

const ImWchar EMPTY_FONT_RANGE[] = {
    0 // end
};

void NewPauseScreen::InitIMGui_BundledFont() {
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
    atlas->TexUvLines[i] = FontAtlas::TexUvLines[i];
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
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
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
    font->Glyphs.push_back(v);
  }
  font->Ascent = FontAtlas::ASCENT;
  font->Descent = FontAtlas::DESCENT;
  font->MetricsTotalSurface = FontAtlas::MetricsTotalSurface;
  for (int i = 0; i < sizeof(FontAtlas::Used4kPagesMap); i++) {
    font->Used4kPagesMap[i] = FontAtlas::Used4kPagesMap[i];
  }
  font->ContainerAtlas = io.Fonts;
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

unsigned char *imguiFontTexData;

void NewPauseScreen::InitIMGui_GenerateFont() {
  // init
  ImGui::SetAllocatorFunctions(
      &prime_malloc, &prime_free
  );


  OSReport("Create context \n");
  ImFontAtlas *atlas = new ImFontAtlas();
  atlas->TexDesiredWidth = 512;
  atlas->Flags |= ImFontAtlasFlags_NoMouseCursors;
  ImGui::CreateContext(atlas);

  // Setup basic flags
  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "gx";
  io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
  // TODO: gamepad mapping

//  // setup font
  ImFontConfig fontConfig{};
  fontConfig.SizePixels = 10;
  fontConfig.OversampleH = 2;
  fontConfig.OversampleV = 2;
  fontConfig.PixelSnapH = true;
  fontConfig.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
  // gen font and font data
  io.Fonts->AddFontDefault(&fontConfig);
  unsigned char *texData = nullptr;
  int width, height, bpp;
  io.Fonts->GetTexDataAsAlpha8(&texData, &width, &height, &bpp);
  OSReport("FONT TEX: %d %d %d\n", width, height, bpp);
  u32 ptr = reinterpret_cast<u32>(prime_malloc(width * height / 2 + 32, nullptr));
  // align
  ptr += 32 - ptr % 32;
  imguiFontTexData = reinterpret_cast<unsigned char *>(ptr);
  memset(imguiFontTexData, 0, width * height / 2);
  // swizzle font

  constexpr int blockWidth = 8;
  constexpr int blockHeight = 8;
  const int xBlocks = width / blockWidth;
  const int yBlocks = height / blockHeight;

  for (int xBlock = 0; xBlock < xBlocks; xBlock++) {
    for (int yBlock = 0; yBlock < yBlocks; yBlock++) {
      int blockStartX = xBlock * blockWidth;
      int blockStartY = yBlock * blockHeight;
      int outStart = ((yBlock * xBlocks) + xBlock) * (blockWidth * blockHeight);
      // Ok now loop over the pixels
      for (int blockRelativeX = 0; blockRelativeX < blockWidth; blockRelativeX++) {
        for (int blockRelativeY = 0; blockRelativeY < blockHeight; blockRelativeY++) {

          int blockRelativeOffset = blockRelativeY * blockWidth + blockRelativeX;
          int globalOffset = (blockStartY + blockRelativeY) * width + (blockStartX + blockRelativeX);

          unsigned char pixel = (texData[globalOffset] >> 4) & 0xF;
//          unsigned char pixel = blockRelativeY * 64;
          int pixelPos = outStart + blockRelativeOffset;

          int bytePos = pixelPos / 2;
          if (pixelPos % 2 == 0) {
            imguiFontTexData[bytePos] |= pixel << 4;
          } else {
            imguiFontTexData[bytePos] |= pixel;
          }
        }
      }
      // Done with the block
    }
  }
  // Don't need the tex data anymore
  io.Fonts->ClearTexData();
//  // send it off to GX
  GXInitTexObj(&imguiFontTexture, imguiFontTexData,
               width, height,
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

// entities
/*duk_ret_t script_getEntities(duk_context *ctx) {
  CStateManager *mgr = CStateManager_INSTANCE;
  CObjectList *list = mgr->GetAllObjs();
  if (list == nullptr) {
    duk_push_undefined(ctx);
    return 1;
  }
  duk_uarridx_t visited = 0;
  int id = list->first;

  duk_push_array(ctx);
  while (id != 0xFFFF && visited < list->count) {
    SObjectListEntry entry = list->entries[id & 0x3FF];
    if (!VALID_PTR(entry.entity)) {
      break;
    }
    CEntity *entity = entry.entity;

    duk_push_object(ctx);
    {
      duk_push_int(ctx, entity->getVtablePtr());
      duk_put_prop_string(ctx, -2, "vtable");

      duk_push_int(ctx, (duk_int32_t) entity);
      duk_put_prop_string(ctx, -2, "address");
    }
    duk_put_prop_index(ctx, -2, visited);

    visited++;
    id = entry.next;
  }
  return 1;
}*/
