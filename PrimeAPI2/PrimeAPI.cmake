
set(PRIMEAPI2_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(CMAKE_TOOLCHAIN_FILE "${PRIMEAPI2_PATH}/PrimeToolchain.cmake")

set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

set(GCC_VERSION "15.1.0")

set(CMAKE_PRIME_C_FLAGS_LIST
        -target powerpc-unknown-eabi
        #        -mllvm --relocation-model=static
        -nostdlib
        -nostdinc
        -ffreestanding
        -isystem "${DEVKITPPC}/lib/gcc/powerpc-eabi/${GCC_VERSION}/include"
        -isystem "${DEVKITPPC}/lib/gcc/powerpc-eabi/${GCC_VERSION}/include-fixed"
        -isystem "${DEVKITPPC}/powerpc-eabi/include"
        # be very careful with this one, it can be dangerous. But I want metaprogramming!
        -isystem "${DEVKITPPC}/powerpc-eabi/include/c++/${GCC_VERSION}"
        -isystem "${DEVKITPPC}/powerpc-eabi/include/c++/${GCC_VERSION}/powerpc-eabi"
        -fno-function-sections
        -fno-data-sections
        -fno-exceptions
        -fno-unroll-loops
        -fno-rtti
        -fno-asynchronous-unwind-tables
        -fvisibility=hidden
        -flto=thin
        #        -mno-sdata
)

set(CMAKE_PRIME_CXX_FLAGS_LIST
        ${CMAKE_PRIME_C_FLAGS_LIST}
        -fno-rtti
        -Wno-delete-incomplete
)

set(LD_FILE ${PRIMEAPI2_PATH}/0-00.ld)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(LD_FILE ${PRIMEAPI2_PATH}/debug.ld)
endif ()

set(CMAKE_PRIME_LINK_FLAGS_LIST
        -nostdlib
        --gc-sections
        --no-demangle
        --keep-unique=_earlyboot_memset
        #        "-e _prolog"
        #        "--unresolved-symbols=report-all"
        #        --error-unresolved-symbols
        #        --no-allow-shlib-undefined
        #        --no-undefined
        #        -r
        "-T ${LD_FILE}"
)

list(JOIN CMAKE_PRIME_C_FLAGS_LIST " " CMAKE_PRIME_C_FLAGS)
list(JOIN CMAKE_PRIME_CXX_FLAGS_LIST " " CMAKE_PRIME_CXX_FLAGS)
list(JOIN CMAKE_PRIME_LINK_FLAGS_LIST " " CMAKE_PRIME_LINK_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_PRIME_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_PRIME_CXX_FLAGS}")

include_directories("${DEVKITPRO}/libogc/include/")

# Macro to create a version.h file with the content of the "version = x.y.z" line from a given toml file
macro(create_version_header output_file toml_file)
  get_filename_component(absolute_toml_file "${toml_file}" REALPATH)
  add_custom_command(
          OUTPUT "${output_file}"
          COMMAND python3 "toml_to_version_header.py"
          --input "${absolute_toml_file}"
          --output "${output_file}"
          MAIN_DEPENDENCY "${absolute_toml_file}"
          WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
  )
endmacro()

# Macro to create .obj file containing an arbitrary file as a read-only section
macro(add_data_object output_file input_file section_name)
  get_filename_component(absolute_input_file "${input_file}" REALPATH)

  add_custom_command(
          OUTPUT "${output_file}"
          COMMAND "${OBJCOPY}"
          -I binary
          -O elf32-powerpc
          -B powerpc
          --set-section-flags .data=contents,readonly,noload,data
          --rename-section .data=${section_name}
          "${absolute_input_file}"
          "${output_file}"
          MAIN_DEPENDENCY "${absolute_input_file}"
  )
endmacro()

# Macro to create .obj file containing a symbol list as read-only data
macro(add_symbol_object output_file symbol_list)
  get_filename_component(absolute_symbol_list "${symbol_list}" REALPATH)

  add_custom_command(
          OUTPUT "${output_file}"
          COMMAND python3 "symbols_to_yaml2obj_file.py"
          --llvm-dir "${LLVM_DIR}"
          "${absolute_symbol_list}"
          "${output_file}"
          MAIN_DEPENDENCY "${absolute_symbol_list}"
          WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
  )
endmacro()

# Macro to get the required link arguments in place
macro(add_gc_static_binary name symbol_list base_dol patch_toml bnr_file)
  add_executable(${name} ${ARGN}
          "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols.o"
          "${CMAKE_CURRENT_BINARY_DIR}/patcher_config.o"
          "${CMAKE_CURRENT_BINARY_DIR}/new_bnr.o"
          "${CMAKE_CURRENT_BINARY_DIR}/internal/version.h"
  )
  set_target_properties(${name} PROPERTIES LINK_FLAGS
          "${CMAKE_PRIME_LINK_FLAGS} -Map ${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
  )

  target_link_libraries(${name} "${DEVKITPPC}/lib/gcc/powerpc-eabi/${GCC_VERSION}/libgcc.a")

  # Create the dol_symbols, patcher_config, and new_bnr objects
  add_symbol_object("${CMAKE_CURRENT_BINARY_DIR}/dol_symbols.o" "${symbol_list}")
  add_data_object("${CMAKE_CURRENT_BINARY_DIR}/patcher_config.o" "${patch_toml}" ".patcher_config")
  add_data_object("${CMAKE_CURRENT_BINARY_DIR}/new_bnr.o" "${bnr_file}" ".new_bnr")
  create_version_header("${CMAKE_CURRENT_BINARY_DIR}/internal/version.h" "${patch_toml}")

  # add internal as an additional include directory, and also as a dependency to force rebuilds
  target_include_directories(${name} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/internal/")

  # Create the patched dol
  add_custom_command(
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
          COMMAND "${GCN_STATIC_PATCHER}"
          -m "${CMAKE_CURRENT_BINARY_DIR}/${name}"
          -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
          -o "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
          --overwrite
          DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}" "${CMAKE_CURRENT_BINARY_DIR}/${name}"
  )
  add_custom_target(
          patch_dol ALL
          DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
          SOURCES "${base_dol}"
  )

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
          DESTINATION "files/"
          RENAME "default.dol")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
          DESTINATION "sys/"
          RENAME "main.dol")
endmacro()
