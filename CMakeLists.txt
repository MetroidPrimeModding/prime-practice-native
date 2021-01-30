cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_TOOLCHAIN_FILE PrimeAPI2/PrimeToolchain.cmake)
project(prime-practice-native)

include_directories(PrimeAPI/include)
include_directories(src/)

set(SOURCE_FILES
        PrimeAPI/include/rstl/string.h
        PrimeAPI/include/rstl/rstl.h
        PrimeAPI/include/PrimeAPI.h
        src/include/prime/fnAddr.cpp
        src/include/prime/CStateManager.h
        src/include/dvd.h
        src/include/os.h
        src/include/prime/CMemoryCardSys.hpp
        src/include/MathyTypes.hpp
        src/include/prime/CSimplePool.hpp
        src/include/types.h
        src/include/prime/CPauseScreen.hpp
        src/include/prime/CFinalInput.hpp
        src/include/prime/CSfxManager.hpp
        src/include/prime/CTextGui.hpp
        src/include/prime/CColor.hpp
        src/include/prime/CGraphics.hpp
        src/include/TextRenderer.hpp
        src/include/NewPauseScreen.hpp
        src/include/prime/CGameGlobalObjects.hpp
        src/include/prime/CHealthInfo.h
        src/include/prime/CMemory.h
        src/include/prime/CPlayerGun.h
        src/include/prime/CPlayerState.h
        src/include/prime/CWorld.hpp
        src/include/prime/CPhysicsActor.hpp
        src/include/prime/CPhysicsState.hpp
        src/include/prime/CPlayer.hpp
        src/include/prime/CWorldTransManager.hpp
        src/include/prime/CMain.hpp
        src/include/prime/CMainFlow.hpp
        src/include/prime/CArchitectureMessage.hpp
        src/include/GetField.hpp
        src/include/GX.hpp
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
        PrimeAPI/include/rstl/rstl.h
        PrimeAPI/include/rstl/vector.h
        src/include/prime/CFontEndUI.hpp
        src/include/prime/CArchitectureQueue.hpp
        src/include/prime/CScriptRelay.hpp
        src/include/prime/CScriptDoor.hpp
        src/include/prime/CScriptCameraHint.hpp
        src/include/STriggerRenderConfig.hpp
        src/new.cpp
        src/NewPauseScreen.cpp
        src/TextRenderer.cpp
        src/duk_mem.h
        src/duk_mem.cpp
        src/prime-practice.cpp
        src/libc.c
        src/practice_mod_rust.cpp
        src/practice_mod_rust_input.cpp
        src/UI/Menu.h
        src/UI/Menu.cpp
        src/UI/MainMenu.h
        src/UI/MainMenu.cpp
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

add_prime_library(default-prime-practice default.dol ${SOURCE_FILES})
#target_link_libraries(default-prime-practice
#        "${CMAKE_SOURCE_DIR}/practice_mod_rust/target/powerpc-unknown-linux-gnu/release/libpractice_mod_rust.a"
#        )