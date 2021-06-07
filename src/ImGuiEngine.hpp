#ifndef PRIME_PRACTICE_NATIVE_IMGUIENGINE_HPP
#define PRIME_PRACTICE_NATIVE_IMGUIENGINE_HPP


#include <prime/CScriptTrigger.hpp>
#include <prime/CFinalInput.hpp>
#include <imgui.h>
#include "prime/CStateManager.hpp"

class ImGuiEngine {
public:
  static void ImGui_Init();
  static void ImGui_Init_Style();
  static void ImGui_Render_GX(const ImDrawData *drawData) ;
};


#endif //PRIME_PRACTICE_NATIVE_IMGUIENGINE_HPP
