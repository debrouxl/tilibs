cmake_minimum_required(VERSION 3.12)

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

if(WIN32)
    list(APPEND HEADER_FILES "src/win64/rwp.h")
    list(APPEND SRC_FILES "src/win64/rwp.c")
endif()

# external deps lookup
if(WIN32)
    pkg_check_modules(DEPS REQUIRED glib-2.0)
    list(APPEND DEPS_LIBRARIES "libusb0")
    list(APPEND DEPS_STATIC_LIBRARIES "libusb0")
else()
    pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0>=1.0.16)
    # Needed for the .pc files configured in the function below.
    set(TICABLES_LIBUSB_REQUIRES_PRIVATE "libusb-1.0")
endif()

try_static_libs_if_needed()

# auto-creation of all targets with flags etc.
create_targets_both_lib_types(ticables2)

if(NOT WIN32)
    # additional internal defines
    target_compile_definitions(ticables2_objlib PUBLIC HAVE_LIBUSB_1_0=1 HAVE_LIBUSB10_STRERROR=1 HAVE_TERMIOS_H=1)
endif()

set_target_properties(ticables2_shared PROPERTIES VERSION 8.0.0 SOVERSION 8)

# Takes care of the i18n po/pot/gmo/mo files
if(ENABLE_NLS)
    i18n_mo_from_po_pot()
    add_dependencies(ticables2_objlib potfiles_2)
endif()

# check includes for parallel and serial support
if(LINUX)
    include(CheckIncludeFile)
    CHECK_INCLUDE_FILE(linux/parport.h HAVE_LINUX_PARPORT_H)
    CHECK_INCLUDE_FILE(linux/serial.h HAVE_LINUX_SERIAL_H)
    if (HAVE_LINUX_PARPORT_H)
        target_compile_definitions(ticables2_objlib PUBLIC HAVE_LINUX_PARPORT_H=1)
    endif()
    if (HAVE_LINUX_SERIAL_H)
        target_compile_definitions(ticables2_objlib PUBLIC HAVE_LINUX_SERIAL_H=1)
    endif()
endif()

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
