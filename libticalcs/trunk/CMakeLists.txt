cmake_minimum_required(VERSION 3.12)

project(libticalcs2
        VERSION     1.1.10
        LANGUAGES   CXX)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/backup.cc
        src/calc_00.cc
        src/calc_73.cc
        src/calc_84p.cc
        src/calc_89t.cc
        src/calc_8x.cc
        src/calc_9x.cc
        src/calc_nsp.cc
        src/calc_xx.cc
        src/calclabequipmentdata.cc
        src/clock.cc
        src/cmd68k.cc
        src/cmdz80.cc
        src/dbus_pkt.cc
        src/dirlist.cc
        src/dusb_cmd.cc
        src/dusb_rpkt.cc
        src/dusb_vpkt.cc
        src/error.cc
        src/keys73.cc
        src/keys83.cc
        src/keys83p.cc
        src/keys86.cc
        src/keys89.cc
        src/keys92p.cc
        src/nsp_cmd.cc
        src/nsp_rpkt.cc
        src/nsp_vpkt.cc
        src/probe.cc
        src/romdump.cc
        src/screen.cc
        src/ticalcs.cc
        src/tikeys.cc
        src/type2str.cc
        src/update.cc)

set(PUBLIC_HEADERS
        src/ticalcs.h
        src/export3.h
        src/romdump.h
        src/keys73.h
        src/keys83.h
        src/keys83p.h
        src/keys86.h
        src/keys89.h
        src/keys92p.h
        src/dbus_pkt.h
        src/dusb_rpkt.h
        src/dusb_vpkt.h
        src/dusb_cmd.h
        src/nsp_rpkt.h
        src/nsp_vpkt.h
        src/nsp_cmd.h
        src/cmdz80.h
        src/cmd68k.h
        src/calclabequipmentdata.h)

# external deps lookup
if(TRY_STATIC_LIBS)
    # from libticables and libtifiles...
    if(WIN32)
        set(external_deps_if_static "libarchive")
    else()
        set(external_deps_if_static "libusb-1.0>=1.0.16" "libarchive")
    endif()
endif()
pkg_check_modules(DEPS REQUIRED glib-2.0 ${external_deps_if_static})
if(WIN32)
    list(APPEND DEPS_LIBRARIES "libusb0")
    list(APPEND DEPS_STATIC_LIBRARIES "libusb0")
endif()

try_static_libs_if_needed()

# auto-creation of all targets with flags etc., alongside with internal deps
create_targets_both_lib_types(ticalcs2 tifiles2 ticables2 ticonv)

set_target_properties(ticalcs2_shared PROPERTIES VERSION 13.0.3 SOVERSION 13)

# there are relocation issues when linking BZip2 and zlib on linux:
#   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbz2.a(bzlib.o): relocation R_X86_64_PC32 against symbol `BZ2_crc32Table'
#   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libz.a(deflate.o): relocation R_X86_64_PC32 against symbol `z_errmsg'
#                can not be used when making a shared object; recompile with -fPIC
if(TRY_STATIC_LIBS AND NOT LINUX)
    find_package(BZip2 REQUIRED) # Needed for some reason
    target_link_libraries(ticalcs2_shared ${BZIP2_LIBRARIES})
endif()

# Takes care of the i18n po/pot/gmo/mo files
if(ENABLE_NLS)
    i18n_mo_from_po_pot()
    add_dependencies(ticalcs2_objlib potfiles_3)
endif()

# public export define
target_compile_definitions(ticalcs2_objlib PUBLIC TICALCS_EXPORTS)

# tests
add_subdirectory(tests)
