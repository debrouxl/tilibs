cmake_minimum_required(VERSION 3.5)

project(libticables2
        VERSION     1.3.6
        LANGUAGES   C)

add_definitions(-DPACKAGE="${PROJECT_NAME}" -DVERSION="${PROJECT_VERSION}" -DTICABLES_EXPORTS)

add_definitions(-DHAVE_LIBUSB_1_0=1 -DHAVE_TERMIOS_H=1)

configure_and_install_pc_file(ticables2 ${PROJECT_VERSION})

pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0)
add_definitions(${DEPS_CFLAGS})
link_directories(${DEPS_LIBRARY_DIRS})
link_libraries(${DEPS_LIBRARIES} ${Intl_LIBRARIES})

include_directories(src)

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

add_library(ticables2_objlib OBJECT ${SRC_FILES})
set_target_properties(ticables2_objlib PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(ticables2_shared SHARED $<TARGET_OBJECTS:ticables2_objlib>)
add_library(ticables2_static STATIC $<TARGET_OBJECTS:ticables2_objlib>)

set_target_properties(ticables2_shared ticables2_static PROPERTIES
        OUTPUT_NAME     ticables2
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")

install(TARGETS ticables2_shared ticables2_static
        ARCHIVE         DESTINATION lib
        LIBRARY         DESTINATION lib
        RUNTIME         DESTINATION bin
        PUBLIC_HEADER   DESTINATION include/tilp2)

i18n_mo_from_po_pot()

create_buildandinstall_target(${PROJECT_NAME} ticables2_shared ticables2_static)
