cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
include("PrimeAPI2/PrimeAPI.cmake")
project(prime-practice-native C CXX)

include_directories(PrimeAPI/include)
include_directories(src/include)
include_directories(src)
include_directories(ext/imgui)

set(IMGUI_H_DEFINE [=[-DIMGUI_USER_CONFIG="\"pracc_imconfig.h\""]=])
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IMGUI_H_DEFINE}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IMGUI_H_DEFINE}")

message(${CMAKE_CXX_FLAGS})

set(SOURCE_FILES
        PrimeAPI/include/PrimeAPI.h

        PrimeAPI/include/rstl/string.h
        PrimeAPI/include/rstl/rstl.h
        PrimeAPI/include/rstl/vector.h
        src/include/dvd.h
        src/include/types.h
        src/include/os.h

        src/include/MathyTypes.hpp
        src/include/prime/fnAddr.cpp
        src/include/prime/CStateManager.hpp
        src/include/prime/CMemoryCardSys.hpp
        src/include/prime/CSimplePool.hpp
        src/include/prime/CPauseScreen.hpp
        src/include/prime/CFinalInput.hpp
        src/include/prime/CSfxManager.hpp
        src/include/prime/CTextGui.hpp
        src/include/prime/CColor.hpp
        src/include/prime/CGraphics.hpp
        src/include/prime/CGameGlobalObjects.hpp
        src/include/prime/CHealthInfo.hpp
        src/include/prime/CMemory.hpp
        src/include/prime/CPlayerGun.hpp
        src/include/prime/CPlayerState.hpp
        src/include/prime/CWorld.hpp
        src/include/prime/CPhysicsActor.hpp
        src/include/prime/CPhysicsState.hpp
        src/include/prime/CPlayer.hpp
        src/include/prime/CWorldTransManager.hpp
        src/include/prime/CMain.hpp
        src/include/prime/CMainFlow.hpp
        src/include/prime/CArchitectureMessage.hpp
        src/include/prime/CMFGame.hpp
        src/include/prime/CIOWinManager.hpp
        src/include/prime/CGameAllocator.hpp
        src/include/prime/CDvdFile.hpp
        src/include/prime/CIOWin.hpp
        src/include/prime/CObjectList.hpp
        src/include/prime/CActor.hpp
        src/include/prime/CScriptTrigger.hpp
        src/include/prime/CScriptDock.hpp
        src/include/prime/CEntity.hpp
        src/include/prime/CFontEndUI.hpp
        src/include/prime/CArchitectureQueue.hpp
        src/include/prime/CScriptRelay.hpp
        src/include/prime/CScriptDoor.hpp
        src/include/prime/CScriptCameraHint.hpp
        src/include/STriggerRenderConfig.hpp
        src/include/NewPauseScreen.hpp
        src/include/GetField.hpp
        src/include/GX.hpp

        ext/imgui/imgui.cpp
#        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui_draw.cpp
        ext/imgui/imgui_tables.cpp
        ext/imgui/imgui_widgets.cpp
        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui.h
        ext/imgui/imgui_internal.h
#        imgui/imgui_draw.cpp

        src/include/pracc_imconfig.h
        src/CrashAssert.h
        src/CrashAssert.cpp
        src/ImGuiEngine.h
        src/ImGuiEngine.cpp
        src/libc/printf.h
        src/libc/printf.c
        src/libc/printf_helper.c
        src/libc/sscanf.c

        src/new.cpp
        src/NewPauseScreen.cpp
        src/duk_mem.h
        src/duk_mem.cpp
        src/prime-practice.cpp
        src/libc/libc.c
        src/UI/WarpMenu.h
        src/UI/WarpMenu.cpp
        src/UI/PlayerMenu.hpp
        src/UI/PlayerMenu.cpp
        src/UI/InventoryMenu.hpp
        src/UI/InventoryMenu.cpp
        )

patch_function("CGraphics::EndScene()"
        "RenderHook()")
patch_function("CPauseScreen::Draw()"
        "PauseScreenDrawReplacement(CPauseScreen *)")
patch_function("CPauseScreen::ProcessControllerInput(const CStateManager &, const CFinalInput &)"
        "PauseControllerInputHandler(CPauseScreen *, CStateManager &, const CFinalInput &)")
patch_function("CMainFlow::OnMessage(const CArchitectureMessage&, CArchitectureQueue&)"
        "IOWinMessageHook(CMainFlow *, const CArchitectureMessage &, CArchitectureQueue &)")
patch_function("CStateManager::DrawDebugStuff() const"
        "drawDebugStuff(CStateManager *)")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  patch_function("CMainFlow::AdvanceGameState(CArchitectureQueue&)"
          "Hook_CMainFlow_AdvanceGameState(CMainFlow*, CArchitectureQueue&)")
endif()

add_prime_library(prime-practice src/prime-practice.lst default.dol ${SOURCE_FILES})
#target_link_libraries(default-prime-practice
#        "${CMAKE_SOURCE_DIR}/practice_mod_rust/target/powerpc-unknown-linux-gnu/release/libpractice_mod_rust.a"
#        )
