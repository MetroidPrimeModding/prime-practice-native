#pragma once


#include <prime/CScriptTrigger.hpp>
#include <prime/CFinalInput.hpp>
#include <imgui.h>
#include "prime/CStateManager.hpp"

#define USE_GX

struct IMGuiGXTexture {
  GXTexObj *obj = nullptr;
  GXTlutObj *tlut = nullptr;
  int tlut_name = GX_TLUT0;
};

class ImGuiEngine {
public:
  static void ImGui_Init();
  static void ImGui_Init_Style();
  static void ImGui_Render(const ImDrawData *drawData);
#ifdef USE_GX
  static void ImGui_Render_GX(const ImDrawData *drawData);
#else
  static void ImGui_Render_Stream(const ImDrawData *drawData);
#endif
};
