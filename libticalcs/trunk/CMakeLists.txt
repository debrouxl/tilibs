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
        src/cmd68k.h)

# auto-creation of all targets with flags etc., alongside with internal deps
create_lib_target_with_deps(ticalcs2 tifiles2 ticables2 ticonv)

if(LINUX)
    target_link_libraries(ticalcs2 PRIVATE Threads::Threads)
endif()

# external deps lookup
if(WIN32)
    find_library(LIBUSB0 NAMES libusb0.lib usb0 REQUIRED)
    target_link_libraries(ticalcs2 PRIVATE ${LIBUSB0})
else()
    pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0>=1.0.16)
    target_link_libraries(ticalcs2 PRIVATE PkgConfig::libusb)
endif()
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
find_package(ZLIB REQUIRED) # for libtifiles
find_package(LibArchive REQUIRED) # for libticables
target_link_libraries(ticalcs2 PRIVATE ZLIB::ZLIB PkgConfig::glib LibArchive::LibArchive)

set_target_properties(ticalcs2 PROPERTIES VERSION 13.0.3 SOVERSION 13)

# Takes care of the i18n po/pot/gmo/mo files
if(ENABLE_NLS)
    i18n_mo_from_po_pot()
    add_dependencies(ticalcs2 potfiles_3)
endif()

# public export define
target_compile_definitions(ticalcs2 PUBLIC TICALCS_EXPORTS)

# tests
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
