cmake_minimum_required(VERSION 3.5)

project(libticalcs2
        VERSION     1.1.10
        LANGUAGES   C)

add_definitions(-DPACKAGE="${PROJECT_NAME}" -DVERSION="${PROJECT_VERSION}" -DTICALCS_EXPORTS)

pkg_check_modules(DEPS REQUIRED glib-2.0 ticonv ticables2 tifiles2)
add_definitions(${DEPS_CFLAGS})
link_directories(${DEPS_LIBRARY_DIRS})
link_libraries(${DEPS_LIBRARIES})

include_directories(src)

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

add_library(ticalcs2_objlib OBJECT ${SRC_FILES})
set_target_properties(ticalcs2_objlib PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(ticalcs2_shared SHARED $<TARGET_OBJECTS:ticalcs2_objlib>)
add_library(ticalcs2_static STATIC $<TARGET_OBJECTS:ticalcs2_objlib>)

set_target_properties(ticalcs2_shared PROPERTIES OUTPUT_NAME ticalcs2)
set_target_properties(ticalcs2_static PROPERTIES OUTPUT_NAME ticalcs2)

install(TARGETS ticalcs2_shared ticalcs2_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
