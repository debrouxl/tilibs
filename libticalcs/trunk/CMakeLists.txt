cmake_minimum_required(VERSION 3.5)

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

# external deps lookup
pkg_check_modules(DEPS REQUIRED glib-2.0 ticonv ticables2 tifiles2)

# auto-creation of all targets with flags etc., alongside with internal deps
create_targets_both_lib_types(ticalcs2 tifiles2 ticables2 ticonv)

# Takes care of the i18n po/pot/gmo/mo files
i18n_mo_from_po_pot()
add_dependencies(ticalcs2_objlib potfiles_3)

# internal deps
add_dependencies(ticalcs2_objlib buildandinstall_libticonv buildandinstall_libticables2 buildandinstall_libtifiles2)

# public export define
target_compile_definitions(ticables2_objlib PUBLIC TICALCS_EXPORTS)

# tests
add_subdirectory(tests)
