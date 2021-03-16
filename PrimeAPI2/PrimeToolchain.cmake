# Hanafuda toolchain
set(CMAKE_SYSTEM_NAME powerpc-elf-prime)

if (NOT DEFINED LLVM_DIR)
    message(FATAL_ERROR "Must specify LLVM_DIR")
endif()

# Set toolchain programs
set(CMAKE_C_COMPILER "${LLVM_DIR}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_DIR}/bin/clang++")

# Set triple for CMake's identification
set(triple powerpc-none-unknown-elf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# Sysroont
#set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}")

# Skip test compile (hanafuda has a somewhat unorthodox compiler workflow)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Compile a C file into an object file
set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> -c <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> -c <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

# Link object files to an executable
set(CMAKE_C_LINK_EXECUTABLE "${LLVM_DIR}/bin/ld.lld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_DIR}/bin/ld.lld <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")

# Thing that doesn't work
set(CMAKE_C_OUTPUT_EXTENSION ".o")
set(CMAKE_CXX_OUTPUT_EXTENSION ".o")
