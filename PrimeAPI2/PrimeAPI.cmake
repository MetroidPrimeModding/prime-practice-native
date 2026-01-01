
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
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(LD_FILE ${PRIMEAPI2_PATH}/debug.ld)
endif()

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

# Macro to get the required link arguments in place
macro(add_prime_binary name symbol_list base_dol)
    add_executable(${name} ${ARGN}
            "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols.o"
    )
    set_target_properties(${name} PROPERTIES LINK_FLAGS
            "${CMAKE_PRIME_LINK_FLAGS} -Map ${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
    )

    target_link_libraries(${name} "${DEVKITPPC}/lib/gcc/powerpc-eabi/${GCC_VERSION}/libgcc.a")

    # Create the dol_symbols.o
    get_filename_component(absolute_symbol_list "${symbol_list}" REALPATH)

    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols.o"
            COMMAND python3 "symbols_to_yaml2obj_file.py"
            --llvm-dir "${LLVM_DIR}"
            "${absolute_symbol_list}"
            "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols.o"
            MAIN_DEPENDENCY "${absolute_symbol_list}"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )

    # Create the patched dol
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            COMMAND python3 "main.py"
            dol
            -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            -m "${CMAKE_CURRENT_BINARY_DIR}/${name}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}" "${CMAKE_CURRENT_BINARY_DIR}/${name}"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/../patcher/"
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
