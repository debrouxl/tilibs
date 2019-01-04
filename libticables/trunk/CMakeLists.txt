cmake_minimum_required(VERSION 3.5)

project(libticables2
        VERSION     1.3.6
        LANGUAGES   C)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/data_log.c
        src/detect.c
        src/error.c
        src/hex2dbus.c
        src/hex2dusb.c
        src/hex2nsp.c
        src/ioports.c
        src/link_blk.c
        src/link_gry.c
        src/link_nul.c
        src/link_par.c
        src/link_tcpc.c
        src/link_tcps.c
        src/link_tie.c
        src/link_usb.c
        src/link_vti.c
        src/link_xxx.c
        src/log_dbus.c
        src/log_dusb.c
        src/log_hex.c
        src/log_nsp.c
        src/none.c
        src/probe.c
        src/ticables.c
        src/type2str.c)

set(PUBLIC_HEADERS
        src/ticables.h
        src/export1.h
        src/timeout.h)

# external deps lookup
pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0)

# auto-creation of all targets with flags etc., alongside with internal deps
create_targets_both_lib_types(ticables2)

# Takes care of the i18n po/pot/gmo/mo files
i18n_mo_from_po_pot()
add_dependencies(ticables2_objlib potfiles_2)

# additional internal defines
target_compile_definitions(ticables2_objlib PUBLIC HAVE_LIBUSB_1_0=1 HAVE_TERMIOS_H=1)

# public export define
target_compile_definitions(ticables2_objlib PUBLIC TICABLES_EXPORTS)

option(ENABLE_LOGGING "Whether to build with cables logging enabled (default ON)" ON)
if(ENABLE_LOGGING)
    target_compile_definitions(ticables2_objlib PUBLIC ENABLE_LOGGING=1)
else()
    message("Building ${PROJECT_NAME} with logging disabled")
endif()

# tests
add_subdirectory(tests)
