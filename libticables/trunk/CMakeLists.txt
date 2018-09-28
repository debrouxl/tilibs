cmake_minimum_required(VERSION 3.5)

project(libticables2
        VERSION     1.3.6
        LANGUAGES   CXX)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/data_log.cc
        src/detect.cc
        src/error.cc
        src/hex2dbus.cc
        src/hex2dusb.cc
        src/hex2nsp.cc
        src/ioports.cc
        src/link_blk.cc
        src/link_gry.cc
        src/link_nul.cc
        src/link_par.cc
        src/link_tcpc.cc
        src/link_tcps.cc
        src/link_tie.cc
        src/link_usb.cc
        src/link_vti.cc
        src/link_xxx.cc
        src/log_dbus.cc
        src/log_dusb.cc
        src/log_hex.cc
        src/log_nsp.cc
        src/none.cc
        src/probe.cc
        src/ticables.cc
        src/type2str.cc)

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
