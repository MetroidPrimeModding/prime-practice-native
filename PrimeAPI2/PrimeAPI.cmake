
set(PRIMEAPI2_PATH "${CMAKE_CURRENT_LIST_DIR}")

if (NOT DEFINED LLVM_DIR)
    message(FATAL_ERROR "Must specify LLVM_DIR")
endif()
set(CMAKE_TOOLCHAIN_FILE "${PRIMEAPI2_PATH}/PrimeToolchain.cmake")

set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

set(CMAKE_PRIME_C_FLAGS_LIST
        -target powerpc-unknown-eabi
        -mllvm --relocation-model=static
        -nostdlib
        -nostdinc
        -ffreestanding
        -fno-function-sections
        -fno-data-sections
        -fno-exceptions
        -fno-asynchronous-unwind-tables
        -fvisibility=hidden
        -flto=thin
        )

set(CMAKE_PRIME_CXX_FLAGS_LIST
        ${CMAKE_PRIME_C_FLAGS_LIST}
        -fno-rtti
        -Wno-delete-incomplete
        )

set(CMAKE_PRIME_LINK_FLAGS_LIST
        -nostdlib
        --gc-sections
        "-e __rel_prolog"
        "--unresolved-symbols=report-all"
        --error-unresolved-symbols
        --no-allow-shlib-undefined
        --no-undefined
        -r
        "-T ${PRIMEAPI2_PATH}/eppc.ld"
        )

list(JOIN CMAKE_PRIME_C_FLAGS_LIST " " CMAKE_PRIME_C_FLAGS)
list(JOIN CMAKE_PRIME_CXX_FLAGS_LIST " " CMAKE_PRIME_CXX_FLAGS)
list(JOIN CMAKE_PRIME_LINK_FLAGS_LIST " " CMAKE_PRIME_LINK_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_PRIME_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_PRIME_CXX_FLAGS}")

# Macro to get the required link arguments in place
macro(add_prime_library_common name symbol_list base_dol)
    add_executable(${name} ${ARGN}
            "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches-${name}.cpp"
            "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols-${name}.o"
    )
    set_target_properties(${name} PROPERTIES LINK_FLAGS
            "${CMAKE_PRIME_LINK_FLAGS} -Map ${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
    )
    target_include_directories(${name} SYSTEM PUBLIC
        "${DEVKITPRO}/libogc/include"
        "${DEVKITPPC}/lib/gcc/powerpc-eabi/10.2.0/include"
        "${DEVKITPPC}/powerpc-eabi/include"
    )

    # Create the ApplyCodePatches.cpp
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches-${name}.cpp"
            COMMAND python3 "parse_and_generate_patch.py"
            -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches-${name}.cpp"
            ${PRIME_PATCH_FUNCTIONS}
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )

    # Create the dol_symbols.o
    get_filename_component(absolute_symbol_list "${symbol_list}" REALPATH)

    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols-${name}.o"
            COMMAND python3 "symbols_to_yaml2obj_file.py"
            --llvm-dir "${LLVM_DIR}"
            "${absolute_symbol_list}"
            "${CMAKE_CURRENT_BINARY_DIR}/dol_symbols-${name}.o"
            MAIN_DEPENDENCY "${absolute_symbol_list}"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )

    # Create the patched dol
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}-default-mod.dol"
            COMMAND python3 "patch_dol_file.py"
            -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/${name}-default-mod.dol"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )
    add_custom_target(
            ${name}-patch-dol
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}-default-mod.dol"
            SOURCES "${base_dol}"
    )
    add_dependencies(${name} ${name}-patch-dol)


    # Create the Mod.rel
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}-Mod.rel"
            COMMAND cargo run -p dol_linker --
            rel
            -o "${CMAKE_CURRENT_BINARY_DIR}/${name}-Mod.rel"
            -s "${PRIMEAPI2_PATH}/empty.lst"
            "${CMAKE_CURRENT_BINARY_DIR}/${name}"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/randomprime/"
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}"
    )
    add_custom_target(
            ${name}-build-mod ALL
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}-Mod.rel"
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}-Mod.rel"
            DESTINATION "files/"
            RENAME "Mod.rel")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}-default-mod.dol"
            DESTINATION "files/"
            RENAME "default.dol")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}-default-mod.dol"
            DESTINATION "sys/"
            RENAME "main.dol")
endmacro()

macro(add_prime_library name symbol_list base_dol)
    add_prime_library_common("${name}" "${symbol_list}" "${base_dol}" ${ARGN})
    target_compile_definitions(${name} PUBLIC -DPRIME=1)
endmacro()

macro(add_echoes_library name symbol_list base_dol)
    add_prime_library_common("${name}" "${symbol_list}" "${base_dol}" ${ARGN})
    target_compile_definitions(${name} PUBLIC -DPRIME=2)
endmacro()

macro(patch_function orig dest)
    list(APPEND PRIME_PATCH_FUNCTIONS
            -p "'${orig}'" "'${dest}'"
        )
endmacro()
