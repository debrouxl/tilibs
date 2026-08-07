include(CMakeParseArguments)

find_program(Z80_AS NAMES z80-unknown-coff-as DOC "GNU Z80 assembler")
find_program(Z80_LD NAMES z80-unknown-coff-ld DOC "GNU Z80 linker")
find_program(M68K_AS NAMES m68k-linux-gnu-as DOC "GNU m68k assembler")
find_program(M68K_OBJCOPY NAMES m68k-linux-gnu-objcopy DOC "GNU m68k objcopy")

foreach(tool Z80_AS Z80_LD M68K_AS M68K_OBJCOPY)
    if(NOT ${tool})
        message(FATAL_ERROR
            "BUILD_ROM_DUMPERS requires ${tool}; set -D${tool}=/path/to/tool")
    endif()
endforeach()

if(NOT TARGET tifileutil)
    message(FATAL_ERROR "BUILD_ROM_DUMPERS requires the in-tree tifileutil target")
endif()

set(ROM_DUMPER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/romdumpers")
set(ROM_DUMPER_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB ROM_DUMPER_8X_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_8x/*.asm"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_8x/*.inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_8x/ldscript")

function(ticalcs_add_z80_rom_dumper output_var dumper source_dir source_file
         extension variable_name header_name array_name size_name)
    set(options PROTECTED)
    set(one_value_args MARCH)
    set(multi_value_args DEFINES DEPENDS)
    cmake_parse_arguments(DUMPER "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    set(work_dir "${ROM_DUMPER_OUTPUT_DIR}/${dumper}")
    set(object_file "${work_dir}/${dumper}.o")
    set(binary_file "${work_dir}/${dumper}.bin")
    set(variable_file "${work_dir}/${dumper}${extension}")
    set(header_file "${ROM_DUMPER_HEADER_DIR}/${header_name}")

    set(assembler_args)
    foreach(definition ${DUMPER_DEFINES})
        list(APPEND assembler_args --defsym "${definition}=1")
    endforeach()
    if(DUMPER_MARCH)
        list(APPEND assembler_args "-march=${DUMPER_MARCH}")
    endif()

    set(wrap_args)
    if(DUMPER_PROTECTED)
        list(APPEND wrap_args -p)
    endif()

    add_custom_command(
        OUTPUT "${header_file}"
        BYPRODUCTS "${object_file}" "${binary_file}" "${variable_file}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${work_dir}" "${ROM_DUMPER_HEADER_DIR}"
        COMMAND "${Z80_AS}" ${assembler_args} -I "${source_dir}"
                -o "${object_file}" "${source_dir}/${source_file}"
        COMMAND "${Z80_LD}" -T "${source_dir}/ldscript" --oformat=binary
                -o "${binary_file}" "${object_file}"
        COMMAND $<TARGET_FILE:tifileutil> wrap -n "${variable_name}" "${binary_file}"
                ${wrap_args} -o "${variable_file}"
        COMMAND $<TARGET_FILE:tifileutil> dump -f chexarray
                -o "${header_file}" -n "${array_name}" -s "${size_name}" -c 8 -m 64 "${variable_file}"
        DEPENDS ${DUMPER_DEPENDS} tifileutil
        COMMENT "Building ${dumper} ROM dumper"
        VERBATIM)

    set(${output_var} "${header_file}" PARENT_SCOPE)
endfunction()

function(ticalcs_add_m68k_rom_dumper output_var dumper extension variable_name
         header_name array_name size_name)
    set(options)
    set(one_value_args DEFINITION)
    cmake_parse_arguments(DUMPER "${options}" "${one_value_args}" "" ${ARGN})

    set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_9x")
    set(source_file "${source_dir}/romdump_pure_asm.s")
    set(work_dir "${ROM_DUMPER_OUTPUT_DIR}/${dumper}")
    set(object_file "${work_dir}/${dumper}.o")
    set(binary_file "${work_dir}/${dumper}.bin")
    set(variable_file "${work_dir}/${dumper}${extension}")
    set(header_file "${ROM_DUMPER_HEADER_DIR}/${header_name}")

    set(assembler_args -l -m68000)
    if(DUMPER_DEFINITION)
        list(APPEND assembler_args --defsym "${DUMPER_DEFINITION}=1")
    endif()

    add_custom_command(
        OUTPUT "${header_file}"
        BYPRODUCTS "${object_file}" "${binary_file}" "${variable_file}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${work_dir}" "${ROM_DUMPER_HEADER_DIR}"
        COMMAND "${M68K_AS}" ${assembler_args} -o "${object_file}" "${source_file}"
        COMMAND "${M68K_OBJCOPY}" -O binary "${object_file}" "${binary_file}"
        COMMAND $<TARGET_FILE:tifileutil> wrap -n "${variable_name}" "${binary_file}"
                -o "${variable_file}"
        COMMAND $<TARGET_FILE:tifileutil> dump -f chexarray
                -o "${header_file}" -n "${array_name}" -s "${size_name}" -c 8 -m 64 "${variable_file}"
        DEPENDS "${source_file}" "${source_dir}/romdump.h" tifileutil
        COMMENT "Building ${dumper} ROM dumper"
        VERBATIM)

    set(${output_var} "${header_file}" PARENT_SCOPE)
endfunction()

function(ticalcs_add_rom_dumpers target)
    set(romdump_8x_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_8x")
    set(romdump_84p_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_84p_usb")
    set(romdump_834pce_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/romdump_834pce_usb")

    ticalcs_add_z80_rom_dumper(rom73 dump73 "${romdump_8x_dir}" romdump.asm
        .73p ROMDUMP rom73.h romDump73 romDumpSize73 PROTECTED
        DEFINES TI73 CALC_FLASH DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom82 dump82 "${romdump_8x_dir}" romdump.asm
        .82y "$RD" rom82.h romDump82 romDumpSize82
        DEFINES TI82 NEED_DISPLAY_ROUTINES DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom83 dump83 "${romdump_8x_dir}" romdump.asm
        .83p ROMDUMP rom83.h romDump83 romDumpSize83 PROTECTED
        DEFINES TI83 DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom83p dump83p "${romdump_8x_dir}" romdump.asm
        .8xp ROMDUMP rom83p.h romDump8Xp romDumpSize8Xp PROTECTED
        DEFINES TI83P CALC_FLASH CALC_LINK_ASSIST DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom84pc dump84pc "${romdump_8x_dir}" romdump.asm
        .8xp ROMDUMP rom84pc.h romDump84pc romDumpSize84pc PROTECTED
        DEFINES TI84PC CALC_LINK_ASSIST DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom85 dump85 "${romdump_8x_dir}" romdump.asm
        .85s "$RD" rom85.h romDump85 romDumpSize85
        DEFINES TI85 NEED_DISPLAY_ROUTINES DEPENDS ${ROM_DUMPER_8X_SOURCES})
    ticalcs_add_z80_rom_dumper(rom86 dump86 "${romdump_8x_dir}" romdump.asm
        .86p ROMDump rom86.h romDump86 romDumpSize86
        DEFINES TI86 DEPENDS ${ROM_DUMPER_8X_SOURCES})

    set(romdump_84p_sources "${romdump_84p_dir}/romdump.z80" "${romdump_84p_dir}/ldscript")
    ticalcs_add_z80_rom_dumper(rom84p dump84p "${romdump_84p_dir}" romdump.z80
        .8xp ROMDUMP rom84p.h romDump84p romDumpSize84p PROTECTED
        DEFINES TI84P DEPENDS ${romdump_84p_sources})
    ticalcs_add_z80_rom_dumper(rom84pcu dump84pcu "${romdump_84p_dir}" romdump.z80
        .8xp ROMDUMP rom84pcu.h romDump84pcu romDumpSize84pcu PROTECTED
        DEFINES TI84PC DEPENDS ${romdump_84p_sources})

    set(romdump_834pce_sources "${romdump_834pce_dir}/romdump.z80" "${romdump_834pce_dir}/ldscript")
    ticalcs_add_z80_rom_dumper(rom834pceu dump834pceu "${romdump_834pce_dir}" romdump.z80
        .8xp ROMDUMP rom834pceu.h romDump834pceu romDumpSize834pceu PROTECTED
        MARCH ez80+adl DEFINES TI834PCE DEPENDS ${romdump_834pce_sources})

    ticalcs_add_m68k_rom_dumper(rom89 romdump .89z romdump
        rom89.h romDump89 romDumpSize89)
    ticalcs_add_m68k_rom_dumper(rom92 romdumpf .92p romdump
        rom92f2.h romDump92 romDumpSize92 DEFINITION FARGO)
    ticalcs_add_m68k_rom_dumper(rom89t romdumpu .89z romdump
        rom89t.h romDump89t romDumpSize89t DEFINITION DUSB_DUMPER)

    set(z80_headers
        "${rom73}" "${rom82}" "${rom83}" "${rom83p}" "${rom84pc}"
        "${rom85}" "${rom86}" "${rom84p}" "${rom84pcu}" "${rom834pceu}")
    set(m68k_headers "${rom89}" "${rom92}" "${rom89t}")
    set(generated_headers ${z80_headers} ${m68k_headers})

    set_source_files_properties(${generated_headers} PROPERTIES GENERATED TRUE)
    add_custom_target(z80_rom_dumpers DEPENDS ${z80_headers})
    add_custom_target(m68k_rom_dumpers DEPENDS ${m68k_headers})
    add_custom_target(rom_dumpers DEPENDS z80_rom_dumpers m68k_rom_dumpers)

    target_sources(${target} PRIVATE ${generated_headers})
    add_dependencies(${target} rom_dumpers)
endfunction()
