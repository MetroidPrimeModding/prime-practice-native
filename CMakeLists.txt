cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
#if (NOT CLION)
#    set(CMAKE_TOOLCHAIN_FILE HanafudaToolchain.cmake)
#endif ()
project(prime-practice-native)

add_compile_options(-D CLION)

include_directories(PrimeAPI/include)
include_directories(src/)
include_directories(/opt/devkitpro/libogc/include/)

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
        src/include/UI/UIElement.hpp
        src/include/UI/UITextElement.hpp
        src/include/UI/UIElementList.hpp
        src/include/UI/UIHeapUsageElement.hpp
        src/include/GX.hpp
        src/include/ReadFile.hpp
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
        src/include/prime/CScriptRelay.hpp
        src/UI/UIElement.cpp
        src/UI/UITextElement.cpp
        src/UI/UIHeapUsageElement.cpp
        src/NewPauseScreen.cpp
        src/TextRenderer.cpp
        src/duk_mem.h
        src/duk_mem.cpp
        src/duk_config.h
        src/duktape.h
        src/prime-practice.cpp
        src/readFile.cpp
        src/libc.c
        )

add_executable(default-prime-practice ${SOURCE_FILES})
