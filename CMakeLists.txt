cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
include("PrimeAPI2/PrimeAPI.cmake")
project(prime-practice-native C CXX)

#include_directories(PrimeAPI/include)
include_directories(src/include)
include_directories(src)
include_directories(ext/imgui)
include_directories(ext/QRCode/src)

set(IMGUI_H_DEFINE [=[-DIMGUI_USER_CONFIG="\"pracc_imconfig.h\""]=])
set(QR_DEFINE "-DLOCK_VERSION=4")
#set(QR_DEFINE "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IMGUI_H_DEFINE} ${QR_DEFINE}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IMGUI_H_DEFINE} ${QR_DEFINE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
endif()


message(${CMAKE_CXX_FLAGS})

set(SOURCE_FILES
        src/include/PrimeAPI.h
        src/include/rstl/string.h
        src/include/rstl/rstl.h
        src/include/rstl/vector.h
        src/include/rstl/auto_ptr.h
        src/include/rstl/list.h
        src/include/dvd.h
        src/include/types.h
        src/include/os.h
        src/include/utils.hpp

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
        src/include/prime/CScriptTimer.hpp
        src/include/prime/CScriptTrigger.hpp
        src/include/prime/CScriptDock.hpp
        src/include/prime/CEntity.hpp
        src/include/prime/CFontEndUI.hpp
        src/include/prime/CArchitectureQueue.hpp
        src/include/prime/CScriptRelay.hpp
        src/include/prime/CScriptDoor.hpp
        src/include/prime/CScriptCameraHint.hpp
        src/include/prime/CCameraBobber.hpp
        src/include/prime/CCameraManager.hpp
        src/include/prime/CGameCamera.hpp
        src/include/prime/CFirstPersonCamera.hpp
        src/include/prime/CBallCamera.hpp
        src/include/prime/CResFactory.hpp
        src/include/prime/CAutoMapper.hpp
        src/include/prime/CInGameGuiManager.hpp
        src/include/prime/CGameArchitectureSupport.hpp
        src/include/prime/CRandom16.hpp
        src/include/STriggerRenderConfig.hpp
        src/include/NewPauseScreen.hpp
        src/include/GetField.hpp
        src/include/settings.hpp
        src/include/ImHelpers.hpp
        src/include/GX.hpp

        ext/imgui/imgui.cpp
#        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui_draw.cpp
        ext/imgui/imgui_tables.cpp
        ext/imgui/imgui_widgets.cpp
#        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui.h
        ext/imgui/imgui_internal.h
#        imgui/imgui_draw.cpp

        ext/QRCode/src/qrcode.c

        src/include/pracc_imconfig.h
        src/CrashAssert.h
        src/CrashAssert.cpp
        src/libc/printf.h
        src/libc/printf.c
        src/libc/sscanf.c

        src/version.h
        src/new.cpp
        src/NewPauseScreen.cpp
        src/duk_mem.h
        src/duk_mem.cpp
        src/prime-practice.cpp
        src/settings.cpp
        src/font_atlas.hpp
        src/ImGuiEngine.cpp
        src/ImGuiEngine.hpp
        src/libc/libc.c

        src/UI/WarpMenu.h
        src/UI/WarpMenu.cpp
        src/UI/PlayerMenu.hpp
        src/UI/PlayerMenu.cpp
        src/UI/InventoryMenu.hpp
        src/UI/InventoryMenu.cpp
        src/UI/MonitorWindow.hpp
        src/UI/MonitorWindow.cpp
        src/UI/SettingsMenu.cpp
        src/UI/SettingsMenu.hpp
        src/UI/BombJumping.hpp
        src/UI/BombJumping.cpp
        src/UI/RoomMenu.cpp
        src/UI/RoomMenu.hpp
        src/UI/QR.cpp
        src/UI/QR.hpp

        src/world/WorldRenderer.cpp
        src/world/WorldRenderer.hpp
        )

add_prime_binary(prime-practice src/prime-practice.lst default.dol ${SOURCE_FILES})
