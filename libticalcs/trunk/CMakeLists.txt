cmake_minimum_required(VERSION 3.5)

project(libticalcs2
        VERSION     1.1.10
        LANGUAGES   C)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/backup.c
        src/calc_00.c
        src/calc_73.c
        src/calc_84p.c
        src/calc_89t.c
        src/calc_8x.c
        src/calc_9x.c
        src/calc_nsp.c
        src/calc_xx.c
        src/clock.c
        src/cmd68k.c
        src/cmdz80.c
        src/dbus_pkt.c
        src/dirlist.c
        src/dusb_cmd.c
        src/dusb_rpkt.c
        src/dusb_vpkt.c
        src/error.c
        src/keys73.c
        src/keys83.c
        src/keys83p.c
        src/keys86.c
        src/keys89.c
        src/keys92p.c
        src/nsp_cmd.c
        src/nsp_rpkt.c
        src/nsp_vpkt.c
        src/probe.c
        src/romdump.c
        src/screen.c
        src/ticalcs.c
        src/tikeys.c
        src/type2str.c
        src/update.c)

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
