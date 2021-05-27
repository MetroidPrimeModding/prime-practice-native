#include <prime/CScriptDoor.hpp>
#include <prime/CScriptCameraHint.hpp>
#include <STriggerRenderConfig.hpp>
#include <UI/MainMenu.h>
#include "UI/Menus.h"
#include "os.h"
#include "NewPauseScreen.hpp"
#include "TextRenderer.hpp"
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

#define PAD_MAX_CONTROLLERS 4

GXTexObj imguiFontTexture;

NewPauseScreen *NewPauseScreen::instance = NULL;

NewPauseScreen::NewPauseScreen() {
  OSReport("Hello, Dolphin\n");

  TextRenderer::Init();
  InitIMGui();
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
  if (active) {
    frames++;
  }


//  // Setup for GX
//  // Prime's formats, which are what I want so that's convenient:
//  // 0 9 1 4 0 GX_VTXFMT0 GX_VA_POS   GX_POS_XYZ  GX_F32    0
//  // 0 a 0 4 0 GX_VTXFMT0 GX_VA_NRM   GX_NRM_XYZ  GX_F32    0
//  // 0 b 1 5 0 GX_VTXFMT0 GX_VA_CLR0  GX_CLR_RGBA GX_RGBA8  0
//  // 0 d 1 4 0 GX_VTXFMT0 GX_VA_TEX0  GX_TEX_ST   GX_F32    0
//  // Repeat for all GX_VA_TEXs
//
//  CGraphics::DisableAllLights();
//  CGX::SetZMode(false, GxCompare_NEVER, false);
////  GXSetBlendMode(
////      GX_BM_BLEND,
////      GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
////      GX_LO_NOOP
////  );
//  CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
////  CGraphics::SetAlphaCompare(ERglAlphaFunc_GREATER, 0, ERglAlphaOp_OR, ERglAlphaFunc_GREATER, 0);
//  CGraphics::SetCullMode(ERglCullMode_None);
//  GXLoadTexObj(&fontTexture, GX_TEXMAP0);
//
//  CGX::SetNumTevStages(1);
//  CGX::SetTevOrder(
//      GXTevStage0,
//      GXTexCoord0,
//      GXTexMap0,
//      GXChannelColor0A0
//  );
//  CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_TEXC, GxTevColorArg_RASC, GxTevColorArg_ZERO);
//  CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_TEXA, GxTevAlphaArg_RASA, GxTevAlphaArg_ZERO);
//  CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
//  CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
//
//  CGraphics::SetOrtho(0, 640, 0, 480, -1, 1);
//  CGraphics::SetIdentityModelMatrix();
//  CGraphics::SetIdentityViewPointMatrix();

  //on_frame();
  NewPauseScreen::RenderMenu();
}

void NewPauseScreen::RenderWorld() {
  // TODO: re-implement this
//  STriggerRenderConfig triggerRenderConfig;
//  duk_push_global_object(ctx);
//  {
//    duk_get_prop_string(ctx, -1, "CONFIG");
//    if (duk_is_object(ctx, -1)) {
//      duk_get_prop_string(ctx, -1, "showUnknownTriggers");
//      triggerRenderConfig.renderUnknown = duk_to_boolean(ctx, -1) > 0;
//      duk_pop(ctx);
//
//      duk_get_prop_string(ctx, -1, "showLoadTriggers");
//      triggerRenderConfig.renderLoad = duk_to_boolean(ctx, -1) > 0;
//      duk_pop(ctx);
//
//      duk_get_prop_string(ctx, -1, "showDoorTriggers");
//      triggerRenderConfig.renderDoor = duk_to_boolean(ctx, -1) > 0;
//      duk_pop(ctx);
//
//      duk_get_prop_string(ctx, -1, "showForceTriggers");
//      triggerRenderConfig.renderForce = duk_to_boolean(ctx, -1) > 0;
//      duk_pop(ctx);
//
//      duk_get_prop_string(ctx, -1, "showCameraHintTriggers");
//      triggerRenderConfig.renderCameraHint = duk_to_boolean(ctx, -1) > 0;
//      duk_pop(ctx);
//    }
//    duk_pop(ctx);
//  }
//  duk_pop(ctx);
//
//  if (!triggerRenderConfig.anyOn()) {
//    return;
//  }
//
//  CStateManager *mgr = CStateManager_INSTANCE;
//  CObjectList *list = mgr->GetAllObjs();
//  if (list == nullptr) return;
//
//  SViewport backupViewport = *SVIEWPORT_GLOBAL;
//  mgr->SetupViewForDraw(backupViewport);
//
//  CGraphics::SetCullMode(ERglCullMode_Back);
//  CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
//  CGX::SetZMode(true, GxCompare_LEQUAL, false);
//  CGraphics::SetAlphaCompare(ERglAlphaFunc_GREATER, 0, ERglAlphaOp_OR, ERglAlphaFunc_GREATER, 0);
//  CGraphics::DisableAllLights();
//
//  CGX::SetNumTevStages(1);
//  CGX::SetTevOrder(
//    GXTevStage0,
//    GXTexCoordNull,
//    GXTexMapNull,
//    GXChannelColor0A0
//  );
//  CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_RASC);
//  CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_RASA);
//  CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
//  CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
//
//  CGraphics::StreamBegin(ERglPrimitive_QUADS);
//  int visited = 0;
//  int id = list->first;
//  while (id != 0xFFFF && visited < list->count) {
//    SObjectListEntry entry = list->entries[id & 0x3FF];
//    if (!VALID_PTR(entry.entity)) {
//      break;
//    }
//    CEntity *entity = entry.entity;
//    if (entity->getVtablePtr() == CScriptTrigger::VTABLE_ADDR) {
//      CScriptTrigger *trigger = reinterpret_cast<CScriptTrigger *>(entity);
//      drawTrigger(triggerRenderConfig, list, trigger);
//    }
//    visited++;
//    id = entry.next;
//  }
//
//  CGraphics::StreamEnd();
}

void NewPauseScreen::drawTrigger(const STriggerRenderConfig &config, CObjectList *list, CScriptTrigger *trigger) const {
  if ((trigger->getStatus() & CEntity::ACTIVE_MASK) == 0) {
    return;
  }

  if (*trigger->getTriggerType() == ETriggerType::NotYetDetermined) {
    *trigger->getTriggerType() = determineTriggerType(list, trigger);
  }

  bool render;
  float alpha = 0.05f;
  switch (*trigger->getTriggerType()) {
    case ETriggerType::Load:
      render = config.renderLoad;
      CGraphics::StreamColor(0.2f, 0.6f, 0.3f, alpha);
      break;
    case ETriggerType::Door:
      render = config.renderDoor;
      CGraphics::StreamColor(0.55f, 0.91f, 0.97f, alpha);
      break;
    case ETriggerType::Force:
      render = config.renderForce;
      CGraphics::StreamColor(0.75f, 0.75f, 0.25f, alpha);
      break;
    case ETriggerType::CameraHint:
      render = config.renderCameraHint;
      CGraphics::StreamColor(0.95, 0.6, 0.91, alpha);
      break;
    case ETriggerType::Unknown:
      render = config.renderUnknown;
      CGraphics::StreamColor(0.8f, 0.8f, 0.8f, alpha);
      break;
    case ETriggerType::NotYetDetermined:
      render = true; // should never happen
      CGraphics::StreamColor(0.8f, 0.0f, 0.0f, alpha);
      break;
    default:
      // Should never happen
      render = true;
      CGraphics::StreamColor(0.8f, 0.0f, 0.0f, alpha);
  }

  if (!render) {
    return;
  }

  CTransform4f *transform = trigger->getTransform();
  CVector3f origin = transform->origin();
  CAABox *aabb = trigger->getBounds();
  CVector3f min(
      origin.x + aabb->min.x,
      origin.y + aabb->min.y,
      origin.z + aabb->min.z
  );
  CVector3f max(
      origin.x + aabb->max.x,
      origin.y + aabb->max.y,
      origin.z + aabb->max.z
  );

  //-z
  CGraphics::StreamVertex(max.x, min.y, min.z);
  CGraphics::StreamVertex(max.x, max.y, min.z);
  CGraphics::StreamVertex(min.x, max.y, min.z);
  CGraphics::StreamVertex(min.x, min.y, min.z);

  //+z
  CGraphics::StreamVertex(min.x, min.y, max.z);
  CGraphics::StreamVertex(min.x, max.y, max.z);
  CGraphics::StreamVertex(max.x, max.y, max.z);
  CGraphics::StreamVertex(max.x, min.y, max.z);

  //-x
  CGraphics::StreamVertex(min.x, min.y, min.z);
  CGraphics::StreamVertex(min.x, max.y, min.z);
  CGraphics::StreamVertex(min.x, max.y, max.z);
  CGraphics::StreamVertex(min.x, min.y, max.z);

  //+x
  CGraphics::StreamVertex(max.x, min.y, max.z);
  CGraphics::StreamVertex(max.x, max.y, max.z);
  CGraphics::StreamVertex(max.x, max.y, min.z);
  CGraphics::StreamVertex(max.x, min.y, min.z);

  //-y
  CGraphics::StreamVertex(min.x, min.y, max.z);
  CGraphics::StreamVertex(max.x, min.y, max.z);
  CGraphics::StreamVertex(max.x, min.y, min.z);
  CGraphics::StreamVertex(min.x, min.y, min.z);

  //+y
  CGraphics::StreamVertex(min.x, max.y, min.z);
  CGraphics::StreamVertex(max.x, max.y, min.z);
  CGraphics::StreamVertex(max.x, max.y, max.z);
  CGraphics::StreamVertex(min.x, max.y, max.z);

  CGraphics::FlushStream();
}

CEntity *getEntityWithEditorID(CObjectList *list, u32 objid) {
  int visited = 0;
  u16 uniqueID = list->first;
  while (uniqueID != 0xFFFF && visited < list->count) {
    SObjectListEntry entry = list->entries[uniqueID & 0x3FF];
    if (entry.entity->getEditorID() == objid) {
      return entry.entity;
    }
    visited++;
    uniqueID = entry.next;
  }
  return nullptr;
}

bool entityHasVtableOrIsRelayThatPointsAtVtable(CObjectList *list, u32 objectID, u32 vtable, u32 depth = 0) {
  CEntity *entity = getEntityWithEditorID(list, objectID);
  if (!entity) return false;
  // found the object
  // If we're a relay, recurse
  if (entity->getVtablePtr() == vtable) return true;
  if (entity->getVtablePtr() == CScriptRelay::VTABLE_ADDR & depth < 2) {
    auto connections = entity->getConnections();
    for (u32 i = 0; i < connections->len; i++) {
      auto conn = &connections->ptr[i];
      if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, vtable, depth + 1)) {
        return ETriggerType::Load;
      }
    }
  }
  return false;

}

ETriggerType NewPauseScreen::determineTriggerType(CObjectList *list, CScriptTrigger *trigger) const {
  auto connections = trigger->getConnections();
  for (u32 i = 0; i < connections->len; i++) {
    auto conn = &connections->ptr[i];
    if (conn->x0_state == EScriptObjectState::Entered) {
      if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptDock::VTABLE_ADDR)) {
        return ETriggerType::Load;
      }
    }
    if (conn->x0_state == EScriptObjectState::Inside && conn->x4_msg == EScriptObjectMessage::Open) {
      if (!entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptDoor::VTABLE_ADDR)) {
        return ETriggerType::NotYetDetermined;
      }
      return ETriggerType::Door;
    }
    if (trigger->getForceMagnitude() > 1 || trigger->getForceMagnitude() < -1) {
      return ETriggerType::Force;
    }
    float forceVec = trigger->getForceField()->x * trigger->getForceField()->x
                     + trigger->getForceField()->y * trigger->getForceField()->y
                     + trigger->getForceField()->z * trigger->getForceField()->z;
    if (forceVec > 1) {
      return ETriggerType::Force;
    }
    if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptCameraHint::VTABLE_ADDR)) {
      return ETriggerType::CameraHint;
    }
  }
  return ETriggerType::Unknown;
}

void NewPauseScreen::hide() {
  active = false;
  frames = -1;
}

void NewPauseScreen::show() {
  this->active = true;
  this->frames = 0;
}

void NewPauseScreen::HandleInputs() {
  if (this->active) {
    // TODO
  }
}

//drawing funcs
/*
duk_ret_t script_drawBegin(duk_context *ctx) {
  duk_int_t primitive = duk_require_int(ctx, 0);

  switch (primitive) {
    case ERglPrimitive_POINTS :
    case ERglPrimitive_LINES :
    case ERglPrimitive_LINESTRIP :
    case ERglPrimitive_TRIANGLES :
    case ERglPrimitive_TRIANGLESTRIP :
    case ERglPrimitive_TRIANGLEFAN :
    case ERglPrimitive_QUADS:
      CGraphics::StreamBegin((ERglPrimitive) primitive);
      break;
    default:
      return duk_error(ctx, 1, "Invalid primtive type");
  }

  return 0;
}

duk_ret_t script_drawEnd(duk_context *ctx) {
  CGraphics::StreamEnd();
  return 0;
}

duk_ret_t script_drawFlush(duk_context *ctx) {
  CGraphics::FlushStream();
  return 0;
}

duk_ret_t script_drawVertex(duk_context *ctx) {
  float x = (float) duk_require_number(ctx, 0);
  float y = (float) duk_require_number(ctx, 1);
  float z = (float) duk_require_number(ctx, 2);

  CGraphics::StreamVertex(x, y, z);

  return 0;
}

duk_ret_t script_drawTexcoord(duk_context *ctx) {
  float s = (float) duk_require_number(ctx, 0);
  float t = (float) duk_require_number(ctx, 1);

  CGraphics::StreamTexcoord(s, t);

  return 0;
}

duk_ret_t script_drawColor(duk_context *ctx) {
  float r = (float) duk_require_number(ctx, 0);
  float g = (float) duk_require_number(ctx, 1);
  float b = (float) duk_require_number(ctx, 2);
  float a = (float) duk_require_number(ctx, 3);

  CGraphics::StreamColor(r, g, b, a);

  return 0;
}*/

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
//  if (this->active) {
  this->ImGuiNewFrame();
  ImGui::NewFrame();

  {
    ImGui::Begin(
        "Another Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) {
      // lol todo
    }
    ImGui::End();
  }

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

  GXLoadTexObj(&imguiFontTexture, GX_TEXMAP0);
  CGraphics::StreamBegin(ERglPrimitive_TRIANGLES);
  for (int cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; cmdListIdx++) {
    const ImDrawList *cmdList = drawData->CmdLists[cmdListIdx];
    // For each cmdlist
    for (int cmdBufferIdx = 0; cmdBufferIdx < cmdList->CmdBuffer.Size; cmdBufferIdx++) {
      const ImDrawCmd *cmdBuffer = &cmdList->CmdBuffer[cmdBufferIdx];
      for (int elemIdx = 0; elemIdx < cmdBuffer->ElemCount; elemIdx++) {
        const ImDrawIdx *idx = &cmdList->IdxBuffer[cmdBuffer->IdxOffset + elemIdx];
        const ImDrawVert *dataStart = &cmdList->VtxBuffer[cmdBuffer->VtxOffset + *idx];

        CGraphics::StreamVertex(dataStart->pos.x, 0, dataStart->pos.y);
        CGraphics::StreamTexcoord(dataStart->uv.x, dataStart->uv.y);
        float r = (float) ((dataStart->col >> 16) & 0xFF) / 255.0f;
        float g = (float) ((dataStart->col >> 8) & 0xFF) / 255.0f;
        float b = (float) ((dataStart->col >> 0) & 0xFF) / 255.0f;
        float a = (float) ((dataStart->col >> 24) & 0xFF) / 255.0f;
//        r = g = b = a = 1;
        CGraphics::StreamColor(r, g, b, a);
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
//  }
}

const ImWchar IMGUI_FONT_RANGE[] = {
    1, 255, // extended ascii
    0 // end
};

void NewPauseScreen::InitIMGui() {
  // init
  ImGui::SetAllocatorFunctions(
      &prime_malloc, &prime_free
  );
  OSReport("Create context \n");
  ImGui::CreateContext();

  // Setup basic flags
  ImGuiIO &io = ImGui::GetIO();
  io.BackendRendererName = "gx";
  io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
  // TODO: gamepad mapping

//  // setup font
  ImFontConfig fontConfig{};
  fontConfig.SizePixels = 8;
  fontConfig.GlyphRanges = io.Fonts->GetGlyphRangesDefault();
  // gen font and font data
  io.Fonts->AddFontDefault(&fontConfig);
  unsigned char *texData = nullptr;
  int width, height, bpp;
  //  io.Fonts->GetTexDataAsAlpha8(&texData, &width, &height, &bpp);
  io.Fonts->GetTexDataAsRGBA32(&texData, &width, &height, &bpp);
  OSReport("FONT TEX: %d %d %d %x", width, height, bpp);
  // swizzle font
//  for (int i = 0; i < width * height * 4; i+=4) {
//    char r = texData[i + 2];
//    char g = texData[i + 1];
//    char b = texData[i + 0];
//    char a = texData[i + 3];
//
//    texData[i + 0] = 255; // alpha
//    texData[i + 1] = 255;
//    texData[i + 2] = 255;
//    texData[i + 3] = 255;// green
//  }
//  // send it off to GX
  GXInitTexObj(&imguiFontTexture, texData,
               width, height,
//               GX_TF_I8,
               GX_TF_RGBA8,
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

void NewPauseScreen::ImGuiNewFrame() {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(400, 200);
  io.DeltaTime = 1.f / 60.f;
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
