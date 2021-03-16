
set(PRIMEAPI2_PATH "${CMAKE_CURRENT_LIST_DIR}")

set(CMAKE_TOOLCHAIN_FILE "${PRIMEAPI2_PATH}/PrimeToolchain.cmake")

set(DEVKITPRO "/opt/devkitpro")
set(DEVKITPPC "/opt/devkitpro/devkitPPC")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
set(CMAKE_PRIME_C_FLAGS_LIST
        -target powerpc-unknown-eabi
        -mllvm --relocation-model=static
        -nostdlib
        -nostdinc
        -ffreestanding
        -isystem "${DEVKITPPC}/lib/gcc/powerpc-eabi/10.2.0/include"
        -isystem "${DEVKITPPC}/powerpc-eabi/include"
        -fno-function-sections
        -fno-data-sections
        -fno-exceptions
        -fno-asynchronous-unwind-tables
#        -mno-sdata
        )

set(CMAKE_PRIME_CXX_FLAGS_LIST
        ${CMAKE_PRIME_C_FLAGS_LIST}
        -fno-rtti
        -Wno-delete-incomplete
        )

set(CMAKE_PRIME_LINK_FLAGS_LIST
        -nostdlib
#        -nodefaultlibs
#        -flto
#        -Os
#        -lgcc
#        -lsysbase
        -r
#        -d
#        -x
#        "-z nocopyreloc"
#        "-z combreloc"
#        -call_shared
#        --strip-discarded
#        --gc-sections
        "-e _prolog"
        "--unresolved-symbols=report-all"
        --error-unresolved-symbols
        --no-allow-shlib-undefined
        --no-undefined
        "-T ${PRIMEAPI2_PATH}/eppc.ld"
        )

list(JOIN CMAKE_PRIME_C_FLAGS_LIST " " CMAKE_PRIME_C_FLAGS)
list(JOIN CMAKE_PRIME_CXX_FLAGS_LIST " " CMAKE_PRIME_CXX_FLAGS)
list(JOIN CMAKE_PRIME_LINK_FLAGS_LIST " " CMAKE_PRIME_LINK_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_PRIME_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_PRIME_CXX_FLAGS}")

include_directories("${DEVKITPRO}/libogc/include/")

# Macro to get the required link arguments in place
macro(add_prime_library name base_dol)
    add_executable(${name} ${ARGN}
            "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches.cpp"
            "${PRIMEAPI2_PATH}/python/symbols/prac_mod_symbols.o"
            )
    set_target_properties(${name} PROPERTIES LINK_FLAGS
            "${CMAKE_PRIME_LINK_FLAGS} -Map ${CMAKE_CURRENT_BINARY_DIR}/${name}.map"
            )
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches.cpp"
            COMMAND python3 "parse_and_generate_patch.py"
            -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/ApplyCodePatches.cpp"
            ${PRIME_PATCH_FUNCTIONS}
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            COMMAND python3 "patch_dol_file.py"
            -i "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
    )
    add_custom_target(
            patch_dol
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            SOURCES "${base_dol}"
    )
    add_dependencies(${name} patch_dol)

    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Mod.rel"
            COMMAND python3 "convert_to_rel.py"
            -d "${CMAKE_CURRENT_SOURCE_DIR}/${base_dol}"
            -i "${CMAKE_CURRENT_BINARY_DIR}/${name}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/Mod.rel"
            WORKING_DIRECTORY "${PRIMEAPI2_PATH}/python/"
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}"
    )
    add_custom_target(
            build_mod ALL
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Mod.rel"
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Mod.rel"
            DESTINATION "files/")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            DESTINATION "files/"
            RENAME "default.dol")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/default_mod.dol"
            DESTINATION "sys/"
            RENAME "main.dol")
endmacro()

macro(patch_function orig dest)
    list(APPEND PRIME_PATCH_FUNCTIONS
            -p "'${orig}'" "'${dest}'"
            )
endmacro()