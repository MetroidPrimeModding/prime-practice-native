set(CMAKE_SYSTEM_NAME devkitppc)

if (NOT DEFINED LLVM_DIR)
  # try setting it from environment variable
  if (DEFINED ENV{LLVM_DIR})
    set(LLVM_DIR $ENV{LLVM_DIR})
    message(STATUS "Setting LLVM_DIR from environment variable: ${LLVM_DIR}")
  else ()
    message(FATAL_ERROR "Must specify LLVM_DIR")
  endif ()
endif ()

if (NOT DEFINED LLD_DIR)
  # try setting it from environment variable
  if (DEFINED ENV{LLD_DIR})
    set(LLD_DIR $ENV{LLD_DIR})
    message(STATUS "Setting LLD_DIR from environment variable: ${LLD_DIR}")
  else ()
    message(FATAL_ERROR "Must specify LLD_DIR")
  endif ()
endif ()

# Set toolchain programs
set(CMAKE_C_COMPILER "${LLVM_DIR}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_DIR}/bin/clang++")
set(CMAKE_ASM_COMPILER "${LLVM_DIR}/bin/clang")

# Set triple for CMake's identification
set(triple powerpc-none-unknown-elf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_ASM_COMPILER_TARGET ${triple})

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> -c <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> -c <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> -c <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

set(CMAKE_C_LINK_EXECUTABLE "${LLD_DIR}/bin/ld.lld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLD_DIR}/bin/ld.lld <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")
set(CMAKE_ASM_LINK_EXECUTABLE "${LLD_DIR}/bin/ld.lld <CMAKE_ASM_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")

set(CMAKE_C_OUTPUT_EXTENSION ".o")
set(CMAKE_CXX_OUTPUT_EXTENSION ".o")
set(CMAKE_ASM_OUTPUT_EXTENSION ".o")
