#include <math.h>
#include "include/NewPauseScreen.hpp"
#include <gccore.h>
#include <include/prime/CScriptTrigger.hpp>
#include "include/os.h"
#include "include/prime/CPlayerState.h"
#include "include/prime/CGameGlobalObjects.hpp"
#include "include/prime/CPlayer.hpp"
#include "include/prime/CGraphics.hpp"
#include "include/prime/CWorld.hpp"
#include "include/UI/UITextElement.hpp"
#include "include/UI/UIHeapUsageElement.hpp"
#include "include/prime/CWorldTransManager.hpp"
#include "include/TextRenderer.hpp"
#include "include/prime/CMain.hpp"
#include "include/prime/CSfxManager.hpp"
#include "include/prime/CDvdFile.hpp"
#include "include/GX.hpp"
#include "duktape.h"
#include "duk_mem.h"
#include "include/readFile.hpp"
#include "jskernel.hpp"

#define PAD_MAX_CONTROLLERS 4

NewPauseScreen *NewPauseScreen::instance = NULL;

NewPauseScreen::NewPauseScreen() {
  TextRenderer::Init();
  frames = 0;
  active = false;
  fatalError = nullptr;
  inputs = new CFinalInput[4];
//  hudElement.addChild(new UIHeapUsageElement(10, 460));

//  menuElement.addChild(new UITextElement("Prime practice mod menu", 5, 35));

  this->ctx = duk_create_heap(&prime_malloc, &prime_realloc, &prime_free, nullptr, &script_fatal);
  this->setupScriptFunctions();

  duk_push_string(ctx, "kernel.js");
  if (duk_pcompile_string_filename(ctx, 0, js_kernel) != 0) {
    OSReport("Error compiling kernel: %s\r\n", duk_safe_to_string(ctx, -1));
  } else {
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
      OSReport("Error executing kernel: %s\r\n", duk_safe_to_string(ctx, -1));
    }
  }
  duk_pop(ctx);
}

void NewPauseScreen::Render() {
  frames++;

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
  GXLoadTexObj(&fontTexture, GX_TEXMAP0);

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

  CGraphics::SetOrtho(0, 640, 0, 480, -1, 1);
  CGraphics::SetIdentityModelMatrix();
  CGraphics::SetIdentityViewPointMatrix();

  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "onFrame");

  if (duk_is_function(ctx, -1)) {
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
      OSReport("OnFrame error: %s\r\n", duk_safe_to_string(ctx, -1));
    }
  }
  duk_pop(ctx);

  duk_pop(ctx);

  if (fatalError) {
    TextRenderer::RenderText(fatalError, 100, 100);
  }

  if (active) {
    this->menuElement.draw();
    active = false;
  }
  this->hudElement.draw();
}

void NewPauseScreen::RenderWorld() {
  duk_bool_t renderLoadingTriggers = FALSE;
  duk_push_global_object(ctx);
  {
    duk_get_prop_string(ctx, -1, "CONFIG");
    if (duk_is_object(ctx, -1)) {
      duk_get_prop_string(ctx, -1, "showLoadingTriggers");
      renderLoadingTriggers = duk_to_boolean(ctx, -1);
      duk_pop(ctx);
    }
    duk_pop(ctx);
  }
  duk_pop(ctx);

  CStateManager *mgr = CStateManager_INSTANCE;
  CPlayer *player = mgr->Player();
  CObjectList *list = mgr->GetAllObjs();
  if (list == nullptr) return;

  CGraphics::SetCullMode(ERglCullMode_Back);
  CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
  CGX::SetZMode(true, GxCompare_LEQUAL, false);

  CGX::SetNumTevStages(1);
  CGX::SetTevOrder(
    GXTevStage0,
    GXTexCoordNull,
    GXTexMapNull,
    GXChannelColor0A0
  );
  CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_RASC);
  CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_RASA);
  CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
  CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);

  CGraphics::StreamBegin(ERglPrimitive_QUADS);
  int visited = 0;
  int id = list->first;
  while (id != 0xFFFF && visited < list->count) {
    SObjectListEntry entry = list->entries[id & 0x3FF];
    if (!VALID_PTR(entry.entity)) {
      break;
    }
    CEntity *entity = entry.entity;
    if (entity->getVtablePtr() == 0x803da4d8) {
      CScriptTrigger *trigger = reinterpret_cast<CScriptTrigger *>(entity);
      drawTrigger(trigger);
    }
    visited++;
    id = entry.next;
  }

  CGraphics::StreamEnd();
}

void NewPauseScreen::drawTrigger(CScriptTrigger *trigger) const {
  if ((trigger->getStatus() & CEntity::ACTIVE_MASK) == 0) {
    return;
  }
  ETriggerFlags flags = trigger->getFlags();

  CTransform *transform = trigger->getTransform();
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

  if ((flags & (ETriggerFlags::DetectPlayer))) {
    CGraphics::StreamColor(0.2f, 0.6f, 1.0f, 0.3f);
  } else if ((flags & (ETriggerFlags::DetectMorphedPlayer))) {
    CGraphics::StreamColor(0.2f, 0.1f, 0.6f, 0.3f);
  } else {
    return;
  }

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

void NewPauseScreen::hide() {
  active = false;
  frames = 0;
}

void NewPauseScreen::HandleInputs() {
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "onInput");

  if (duk_is_function(ctx, -1)) {
    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
      OSReport("OnInput error: %s\r\n", duk_safe_to_string(ctx, -1));
    }
  }
  duk_pop(ctx);

  duk_pop(ctx);
}

void NewPauseScreen::setupScriptFunctions() {
  duk_push_global_object(ctx);

  duk_push_c_function(ctx, &script_require, 1);
  duk_put_prop_string(ctx, -2, "nativeRequire");

  duk_push_c_function(ctx, &script_drawText, 3);
  duk_put_prop_string(ctx, -2, "drawText");

  duk_push_c_function(ctx, &script_osReport, 1);
  duk_put_prop_string(ctx, -2, "OSReport");

  duk_push_c_function(ctx, &script_readU32, 1);
  duk_put_prop_string(ctx, -2, "readU32");
  duk_push_c_function(ctx, &script_readS32, 1);
  duk_put_prop_string(ctx, -2, "readS32");
  duk_push_c_function(ctx, &script_readFloat, 1);
  duk_put_prop_string(ctx, -2, "readFloat");
  duk_push_c_function(ctx, &script_readDouble, 1);
  duk_put_prop_string(ctx, -2, "readDouble");

  duk_push_c_function(ctx, &script_writeU32, 2);
  duk_put_prop_string(ctx, -2, "writeU32");
  duk_push_c_function(ctx, &script_writeS32, 2);
  duk_put_prop_string(ctx, -2, "writeS32");
  duk_push_c_function(ctx, &script_writeFloat, 2);
  duk_put_prop_string(ctx, -2, "writeFloat");
  duk_push_c_function(ctx, &script_writeDouble, 2);
  duk_put_prop_string(ctx, -2, "writeDouble");

  duk_push_c_function(ctx, &script_readPadsRaw, 0);
  duk_put_prop_string(ctx, -2, "readPadsRaw");

  duk_push_c_function(ctx, &script_readPads, 0);
  duk_put_prop_string(ctx, -2, "readPads");

  duk_push_c_function(ctx, &script_getGameState, 0);
  duk_put_prop_string(ctx, -2, "getGameState");

  duk_push_c_function(ctx, &script_getPlayer, 0);
  duk_put_prop_string(ctx, -2, "getPlayer");

  duk_push_c_function(ctx, &script_isPauseScreen, 0);
  duk_put_prop_string(ctx, -2, "isPauseScreen");

  duk_push_c_function(ctx, &script_drawBegin, 1);
  duk_put_prop_string(ctx, -2, "drawBegin");

  duk_push_c_function(ctx, &script_drawEnd, 0);
  duk_put_prop_string(ctx, -2, "drawEnd");

  duk_push_c_function(ctx, &script_drawFlush, 0);
  duk_put_prop_string(ctx, -2, "drawFlush");

  duk_push_c_function(ctx, &script_drawVertex, 3);
  duk_put_prop_string(ctx, -2, "drawVertex");

  duk_push_c_function(ctx, &script_drawTexcoord, 2);
  duk_put_prop_string(ctx, -2, "drawTexcoord");

  duk_push_c_function(ctx, &script_drawColor, 4);
  duk_put_prop_string(ctx, -2, "drawColor");

  duk_push_c_function(ctx, &script_setTextColor, 4);
  duk_put_prop_string(ctx, -2, "setTextColor");

  duk_push_c_function(ctx, &script_warp, 2);
  duk_put_prop_string(ctx, -2, "warp");

  duk_push_c_function(ctx, &script_getWorld, 0);
  duk_put_prop_string(ctx, -2, "getWorld");

  duk_push_c_function(ctx, &script_setInventory, 1);
  duk_put_prop_string(ctx, -2, "setInventory");

  duk_push_c_function(ctx, &script_getFPS, 0);
  duk_put_prop_string(ctx, -2, "getFPS");

  duk_push_c_function(ctx, &script_getEntities, 0);
  duk_put_prop_string(ctx, -2, "getEntities");

  duk_pop(ctx);
}

void script_fatal(void *udata, const char *msg) {
  NewPauseScreen::instance->fatalError = msg;
  OSReport("Script Fatal: %s\r\n", msg);
}


duk_ret_t script_require(duk_context *ctx) {
  const char *path = duk_require_string(ctx, 0);

  ReadFileResult res = readFileSync(path);
  OSReport("Script length: %u\r\n", res.len);

  duk_push_string(ctx, path);

  if (duk_pcompile_string_filename(ctx, 0, (char *) res.data) != 0) {
    OSReport("Error compiling %s\r\n", duk_safe_to_string(ctx, -1));
    delete res.data;
    return duk_throw(ctx);
  } else {
    delete res.data;
//    if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
//      OSReport("Error executing file: %s\r\n", duk_safe_to_string(ctx, -1));
//      duk_throw(ctx);
//    }
  }

  return 1;
}

duk_ret_t script_drawText(duk_context *ctx) {
  const char *str = duk_require_string(ctx, 0);
  duk_double_t x = duk_require_number(ctx, 1);
  duk_double_t y = duk_require_number(ctx, 2);

  TextRenderer::RenderText(str, x, y);

  return 0;
}

duk_ret_t script_osReport(duk_context *ctx) {
  const char *str = duk_safe_to_string(ctx, 0);
  OSReport("JS message: %s\r\n", str);
  return 0;
}

duk_ret_t script_readU32(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    duk_uint_t value = *((duk_uint_t *) addr);
    duk_push_uint(ctx, value);
  } else {
    duk_push_uint(ctx, addr);
  }

  return 1;
}

duk_ret_t script_readS32(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    duk_int_t value = *((duk_int_t *) addr);
    duk_push_int(ctx, value);
  } else {
    duk_push_uint(ctx, addr);
  }

  return 1;
}

duk_ret_t script_readFloat(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    float value = *((float *) addr);
    duk_push_number(ctx, value);
  } else {
    duk_push_int(ctx, addr);
  }

  return 1;
}

duk_ret_t script_readDouble(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    double value = *((float *) addr);
    duk_push_number(ctx, value);
  } else {
    duk_push_int(ctx, addr);
  }

  return 1;
}

duk_ret_t script_writeU32(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  double num = duk_require_number(ctx, 1);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    *((u32 *) addr) = (u32) num;
  }

  return 0;
}

duk_ret_t script_writeS32(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  double num = duk_require_number(ctx, 1);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    *((s32 *) addr) = (s32) num;
  }

  return 0;
}

duk_ret_t script_writeFloat(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  double num = duk_require_number(ctx, 1);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    *((float *) addr) = (float) num;
  }

  return 0;
}

duk_ret_t script_writeDouble(duk_context *ctx) {
  double addrDouble = duk_require_number(ctx, 0);
  double num = duk_require_number(ctx, 1);
  uint64 addrLong = (uint64) addrDouble;
  u32 addr = (u32) addrLong;

  if (VALID_PTR(addr)) {
    *((double *) addr) = num;
  }

  return 0;
}

duk_ret_t script_readPadsRaw(duk_context *ctx) {
  PADStatus pads[PAD_MAX_CONTROLLERS];
  PADRead(pads);

  duk_push_array(ctx);
  for (duk_uarridx_t i = 0; i < PAD_MAX_CONTROLLERS; i++) {
    PADStatus pad = pads[i];
    duk_push_object(ctx);

    if (pad.err == PAD_ERR_NONE) {
      // Valid
      duk_push_boolean(ctx, pad.button & PAD_BUTTON_LEFT);
      duk_put_prop_string(ctx, -2, "left");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_RIGHT);
      duk_put_prop_string(ctx, -2, "right");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_DOWN);
      duk_put_prop_string(ctx, -2, "down");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_UP);
      duk_put_prop_string(ctx, -2, "up");

      duk_push_boolean(ctx, pad.button & PAD_TRIGGER_Z);
      duk_put_prop_string(ctx, -2, "z");

      duk_push_boolean(ctx, pad.button & PAD_TRIGGER_R);
      duk_put_prop_string(ctx, -2, "rDigital");

      duk_push_boolean(ctx, pad.button & PAD_TRIGGER_L);
      duk_put_prop_string(ctx, -2, "lDigital");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_A);
      duk_put_prop_string(ctx, -2, "a");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_B);
      duk_put_prop_string(ctx, -2, "b");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_X);
      duk_put_prop_string(ctx, -2, "x");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_Y);
      duk_put_prop_string(ctx, -2, "y");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_MENU);
      duk_put_prop_string(ctx, -2, "menu");

      duk_push_boolean(ctx, pad.button & PAD_BUTTON_START);
      duk_put_prop_string(ctx, -2, "start");

      duk_push_int(ctx, pad.stickX);
      duk_put_prop_string(ctx, -2, "stickX");

      duk_push_int(ctx, pad.stickY);
      duk_put_prop_string(ctx, -2, "stickY");

      duk_push_int(ctx, pad.substickX);
      duk_put_prop_string(ctx, -2, "cX");

      duk_push_int(ctx, pad.substickX);
      duk_put_prop_string(ctx, -2, "cY");

      duk_push_int(ctx, pad.triggerL);
      duk_put_prop_string(ctx, -2, "lAnalog");

      duk_push_int(ctx, pad.triggerR);
      duk_put_prop_string(ctx, -2, "rAnalog");
    } else {
      duk_push_int(ctx, pad.err);
      duk_put_prop_string(ctx, -2, "err");
    }

    duk_put_prop_index(ctx, -2, i);
  }

  return 1;
}

duk_ret_t script_readPads(duk_context *ctx) {
  CFinalInput *inputs = NewPauseScreen::instance->inputs;

  duk_push_array(ctx);
  for (duk_uarridx_t i = 0; i < PAD_MAX_CONTROLLERS; i++) {
    CFinalInput pad = inputs[i];
    duk_push_object(ctx);
    {
      duk_push_object(ctx);
      {
        duk_push_boolean(ctx, pad.DDPLeft());
        duk_put_prop_string(ctx, -2, "left");

        duk_push_boolean(ctx, pad.DDPRight());
        duk_put_prop_string(ctx, -2, "right");

        duk_push_boolean(ctx, pad.DDPDown());
        duk_put_prop_string(ctx, -2, "down");

        duk_push_boolean(ctx, pad.DDPUp());
        duk_put_prop_string(ctx, -2, "up");

        duk_push_boolean(ctx, pad.DZ());
        duk_put_prop_string(ctx, -2, "z");

        duk_push_boolean(ctx, pad.DR());
        duk_put_prop_string(ctx, -2, "rDigital");

        duk_push_boolean(ctx, pad.DL());
        duk_put_prop_string(ctx, -2, "lDigital");

        duk_push_boolean(ctx, pad.DRTrigger());
        duk_put_prop_string(ctx, -2, "rAnalog");

        duk_push_boolean(ctx, pad.DLTrigger());
        duk_put_prop_string(ctx, -2, "lAnalog");

        duk_push_boolean(ctx, pad.DA());
        duk_put_prop_string(ctx, -2, "a");

        duk_push_boolean(ctx, pad.DB());
        duk_put_prop_string(ctx, -2, "b");

        duk_push_boolean(ctx, pad.DX());
        duk_put_prop_string(ctx, -2, "x");

        duk_push_boolean(ctx, pad.DY());
        duk_put_prop_string(ctx, -2, "y");

        duk_push_boolean(ctx, pad.DStart());
        duk_put_prop_string(ctx, -2, "start");

        duk_push_boolean(ctx, pad.DLAUp());
        duk_put_prop_string(ctx, -2, "stickUp");

        duk_push_boolean(ctx, pad.DLADown());
        duk_put_prop_string(ctx, -2, "stickDown");

        duk_push_boolean(ctx, pad.DLALeft());
        duk_put_prop_string(ctx, -2, "stickLeft");

        duk_push_boolean(ctx, pad.DLARight());
        duk_put_prop_string(ctx, -2, "stickRight");

        duk_push_boolean(ctx, pad.DRAUp());
        duk_put_prop_string(ctx, -2, "cUp");

        duk_push_boolean(ctx, pad.DRADown());
        duk_put_prop_string(ctx, -2, "cDown");

        duk_push_boolean(ctx, pad.DRALeft());
        duk_put_prop_string(ctx, -2, "cLeft");

        duk_push_boolean(ctx, pad.DRARight());
        duk_put_prop_string(ctx, -2, "cRight");
      }
      duk_put_prop_string(ctx, -2, "digital");

      duk_push_object(ctx);
      {
        duk_push_boolean(ctx, pad.PDPLeft());
        duk_put_prop_string(ctx, -2, "left");

        duk_push_boolean(ctx, pad.PDPRight());
        duk_put_prop_string(ctx, -2, "right");

        duk_push_boolean(ctx, pad.PDPDown());
        duk_put_prop_string(ctx, -2, "down");

        duk_push_boolean(ctx, pad.PDPUp());
        duk_put_prop_string(ctx, -2, "up");

        duk_push_boolean(ctx, pad.PZ());
        duk_put_prop_string(ctx, -2, "z");

        duk_push_boolean(ctx, pad.PR());
        duk_put_prop_string(ctx, -2, "rDigital");

        duk_push_boolean(ctx, pad.PL());
        duk_put_prop_string(ctx, -2, "lDigital");

        duk_push_boolean(ctx, pad.PRTrigger());
        duk_put_prop_string(ctx, -2, "rAnalog");

        duk_push_boolean(ctx, pad.PLTrigger());
        duk_put_prop_string(ctx, -2, "lAnalog");

        duk_push_boolean(ctx, pad.PA());
        duk_put_prop_string(ctx, -2, "a");

        duk_push_boolean(ctx, pad.PB());
        duk_put_prop_string(ctx, -2, "b");

        duk_push_boolean(ctx, pad.PX());
        duk_put_prop_string(ctx, -2, "x");

        duk_push_boolean(ctx, pad.PY());
        duk_put_prop_string(ctx, -2, "y");

        duk_push_boolean(ctx, pad.PStart());
        duk_put_prop_string(ctx, -2, "start");

        duk_push_boolean(ctx, pad.PLAUp());
        duk_put_prop_string(ctx, -2, "stickUp");

        duk_push_boolean(ctx, pad.PLADown());
        duk_put_prop_string(ctx, -2, "stickDown");

        duk_push_boolean(ctx, pad.PLALeft());
        duk_put_prop_string(ctx, -2, "stickLeft");

        duk_push_boolean(ctx, pad.PLARight());
        duk_put_prop_string(ctx, -2, "stickRight");

        duk_push_boolean(ctx, pad.PRAUp());
        duk_put_prop_string(ctx, -2, "cUp");

        duk_push_boolean(ctx, pad.PRADown());
        duk_put_prop_string(ctx, -2, "cDown");

        duk_push_boolean(ctx, pad.PRALeft());
        duk_put_prop_string(ctx, -2, "cLeft");

        duk_push_boolean(ctx, pad.PRARight());
        duk_put_prop_string(ctx, -2, "cRight");
      }
      duk_put_prop_string(ctx, -2, "pressed");

      duk_push_object(ctx);
      {
        duk_push_number(ctx, pad.ADPLeft());
        duk_put_prop_string(ctx, -2, "left");

        duk_push_number(ctx, pad.ADPRight());
        duk_put_prop_string(ctx, -2, "right");

        duk_push_number(ctx, pad.ADPDown());
        duk_put_prop_string(ctx, -2, "down");

        duk_push_number(ctx, pad.ADPUp());
        duk_put_prop_string(ctx, -2, "up");

        duk_push_number(ctx, pad.AZ());
        duk_put_prop_string(ctx, -2, "z");

        duk_push_number(ctx, pad.AR());
        duk_put_prop_string(ctx, -2, "rDigital");

        duk_push_number(ctx, pad.AL());
        duk_put_prop_string(ctx, -2, "lDigital");

        duk_push_number(ctx, pad.ARTrigger());
        duk_put_prop_string(ctx, -2, "rAnalog");

        duk_push_number(ctx, pad.ALTrigger());
        duk_put_prop_string(ctx, -2, "lAnalog");

        duk_push_number(ctx, pad.AA());
        duk_put_prop_string(ctx, -2, "a");

        duk_push_number(ctx, pad.AB());
        duk_put_prop_string(ctx, -2, "b");

        duk_push_number(ctx, pad.AX());
        duk_put_prop_string(ctx, -2, "x");

        duk_push_number(ctx, pad.AY());
        duk_put_prop_string(ctx, -2, "y");

        duk_push_number(ctx, pad.AStart());
        duk_put_prop_string(ctx, -2, "start");

        duk_push_number(ctx, pad.ALAUp());
        duk_put_prop_string(ctx, -2, "stickUp");

        duk_push_number(ctx, pad.ALADown());
        duk_put_prop_string(ctx, -2, "stickDown");

        duk_push_number(ctx, pad.ALALeft());
        duk_put_prop_string(ctx, -2, "stickLeft");

        duk_push_number(ctx, pad.ALARight());
        duk_put_prop_string(ctx, -2, "stickRight");

        duk_push_number(ctx, pad.ARAUp());
        duk_put_prop_string(ctx, -2, "cUp");

        duk_push_number(ctx, pad.ARADown());
        duk_put_prop_string(ctx, -2, "cDown");

        duk_push_number(ctx, pad.ARALeft());
        duk_put_prop_string(ctx, -2, "cLeft");

        duk_push_number(ctx, pad.ARARight());
        duk_put_prop_string(ctx, -2, "cRight");

        duk_push_number(ctx, pad.ALeftX());
        duk_put_prop_string(ctx, -2, "stickX");

        duk_push_number(ctx, pad.ALeftY());
        duk_put_prop_string(ctx, -2, "stickY");

        duk_push_number(ctx, pad.ARightX());
        duk_put_prop_string(ctx, -2, "cX");

        duk_push_number(ctx, pad.ARightY());
        duk_put_prop_string(ctx, -2, "cY");

        duk_push_number(ctx, pad.ALeftTrigger());
        duk_put_prop_string(ctx, -2, "l");

        duk_push_number(ctx, pad.ARightTrigger());
        duk_put_prop_string(ctx, -2, "r");
      }
      duk_put_prop_string(ctx, -2, "analog");
    }
    duk_put_prop_index(ctx, -2, i);
  }

  return 1;
}

duk_ret_t script_getGameState(duk_context *ctx) {
  CGameGlobalObjects *globals = ((CGameGlobalObjects *) 0x80457798);
  CGameState *gameState = globals->x134_gameState;

  if (gameState) {
    duk_push_object(ctx);
    {
      duk_push_number(ctx, gameState->PlayTime());
      duk_put_prop_string(ctx, -2, "playtime");

      duk_push_number(ctx, gameState->MLVL());
      duk_put_prop_string(ctx, -2, "mlvl");
    }
  } else {
    duk_push_null(ctx);
  }

  return 1;
}

duk_ret_t script_getPlayer(duk_context *ctx) {
  CStateManager *stateManager = CStateManager_INSTANCE;
  CPlayer *player = stateManager->Player();
  CPlayerState *playerState = stateManager->GetPlayerState();

  if (player != nullptr && playerState != nullptr) {
    duk_push_object(ctx);
    {
      duk_push_uint(ctx, (duk_uint_t) player);
      duk_put_prop_string(ctx, -2, "address");

      duk_push_object(ctx);
      {
        CVector3f pos = player->GetPhysicsState().x0_translation;

        duk_push_number(ctx, pos.x);
        duk_put_prop_string(ctx, -2, "x");

        duk_push_number(ctx, pos.y);
        duk_put_prop_string(ctx, -2, "y");

        duk_push_number(ctx, pos.z);
        duk_put_prop_string(ctx, -2, "z");
      }
      duk_put_prop_string(ctx, -2, "pos");

      duk_push_object(ctx);
      {
        CVector3f speed = *player->GetVelocity();

        duk_push_number(ctx, speed.x);
        duk_put_prop_string(ctx, -2, "x");

        duk_push_number(ctx, speed.y);
        duk_put_prop_string(ctx, -2, "y");

        duk_push_number(ctx, speed.z);
        duk_put_prop_string(ctx, -2, "z");
      }
      duk_put_prop_string(ctx, -2, "speed");

      duk_push_object(ctx);
      {
        CVector3f angular = *player->GetAngularVelocity();

        duk_push_number(ctx, angular.x);
        duk_put_prop_string(ctx, -2, "x");

        duk_push_number(ctx, angular.y);
        duk_put_prop_string(ctx, -2, "y");

        duk_push_number(ctx, angular.z);
        duk_put_prop_string(ctx, -2, "z");
      }
      duk_put_prop_string(ctx, -2, "rotation");

      duk_push_array(ctx);
      {
        for (duk_uarridx_t i = 0; i < CPlayerState::kItem_Max; i++) {
          duk_push_int(ctx, playerState->GetPowerups()[(CPlayerState::EItemType) i].mAmount);
          duk_put_prop_index(ctx, -2, i);
        }
      }
      duk_put_prop_string(ctx, -2, "itemAmount");

      duk_push_array(ctx);
      {
        for (duk_uarridx_t i = 0; i < CPlayerState::kItem_Max; i++) {
          duk_push_int(ctx, playerState->GetPowerups()[(CPlayerState::EItemType) i].mCapacity);
          duk_put_prop_index(ctx, -2, i);
        }
      }
      duk_put_prop_string(ctx, -2, "itemCapacity");

      duk_push_int(ctx, playerState->mCurrentSuit);
      duk_put_prop_string(ctx, -2, "currentSuit");

      if (playerState->GetHealthInfo()) {
        duk_push_number(ctx, playerState->GetHealthInfo()->GetHealth());
      } else {
        duk_push_number(ctx, 99);
      }
      duk_put_prop_string(ctx, -2, "health");
    }
  } else {
    duk_push_null(ctx);
  }

  return 1;
}

duk_ret_t script_isPauseScreen(duk_context *ctx) {
  duk_push_boolean(ctx, NewPauseScreen::instance->active);

  return 1;
}

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
}

duk_ret_t script_setTextColor(duk_context *ctx) {
  float r = (float) duk_require_number(ctx, 0);
  float g = (float) duk_require_number(ctx, 1);
  float b = (float) duk_require_number(ctx, 2);
  float a = (float) duk_require_number(ctx, 3);

  TextRenderer::SetColor(r, g, b, a);

  return 0;
}

duk_ret_t script_warp(duk_context *ctx) {
  duk_double_t worldIDDouble = duk_require_number(ctx, 0);
  duk_double_t areaIDDouble = duk_require_number(ctx, 1);
  CAssetId worldID = (CAssetId) ((uint64) worldIDDouble);
  CAssetId areaID = (CAssetId) ((uint64) areaIDDouble);

  CStateManager *mgr = ((CStateManager *) 0x8045A1A8);
  mgr->GetWorld()->SetPauseState(true);

  CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
  CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId);
  gameState->SetCurrentWorldId(worldID);
  gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);

  CMain *cmain = *((CMain **) 0x805A8C38);
  cmain->SetFlowState(EFlowState_None);

  mgr->SetShouldQuitGame(true);

  return 0;
}

duk_ret_t script_getWorld(duk_context *ctx) {
  CStateManager *stateManager = ((CStateManager *) 0x8045A1A8);
  const CWorld *world = stateManager->GetWorld();

  if (world) {
    duk_push_object(ctx);
    {
      duk_push_int(ctx, world->GetCurrentAreaId());
      duk_put_prop_string(ctx, -2, "area");
    }
  } else {
    duk_push_null(ctx);
  }

  return 1;
}

duk_ret_t script_setInventory(duk_context *ctx) {
  CStateManager *stateManager = ((CStateManager *) 0x8045A1A8);
  CPlayer *player = stateManager->Player();
  CPlayerState *playerState = stateManager->GetPlayerState();

  duk_require_object(ctx, 0);

  duk_get_prop_string(ctx, 0, "itemCapacity");
  for (duk_uarridx_t i = 0; i < CPlayerState::kItem_Max; i++) {
    duk_get_prop_index(ctx, -1, i);
    playerState->GetPowerups()[(CPlayerState::EItemType) i].mCapacity = duk_get_int(ctx, -1);
    duk_pop(ctx);
  }
  duk_pop(ctx);
  duk_get_prop_string(ctx, 0, "itemAmount");
  for (duk_uarridx_t i = 0; i < CPlayerState::kItem_Max; i++) {
    duk_get_prop_index(ctx, -1, i);
    playerState->GetPowerups()[(CPlayerState::EItemType) i].mAmount = duk_get_int(ctx, -1);
    duk_pop(ctx);
  }
  duk_pop(ctx);

  duk_get_prop_string(ctx, 0, "health");
  playerState->GetHealthInfo()->SetHealth((float) duk_get_number(ctx, -1));
  duk_pop(ctx);

  duk_get_prop_string(ctx, 0, "currentSuit");
  playerState->mCurrentSuit = (CPlayerState::EPlayerSuit) duk_get_int(ctx, -1);
  duk_pop(ctx);

  return 0;
}

duk_ret_t script_getFPS(duk_context *ctx) {
  duk_push_number(ctx, CGraphics::GetFPS());
  return 1;
}

duk_ret_t script_getEntities(duk_context *ctx) {
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
}