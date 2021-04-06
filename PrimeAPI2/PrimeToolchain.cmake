# Hanafuda toolchain
set(CMAKE_SYSTEM_NAME powerpc-elf-prime)

# Set toolchain programs
set(CMAKE_C_COMPILER "${LLVM_DIR}/bin/clang" CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${LLVM_DIR}/bin/clang++" CACHE FILEPATH "" FORCE)

# Set triple for CMake's identification
set(triple powerpc-none-unknown-elf)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

# Skip test compile (hanafuda has a somewhat unorthodox compiler workflow)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Link object files to an executable
set(CMAKE_C_LINK_EXECUTABLE "${LLVM_DIR}/bin/ld.lld <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")
set(CMAKE_CXX_LINK_EXECUTABLE "${LLVM_DIR}/bin/ld.lld <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET> <OBJECTS>")
