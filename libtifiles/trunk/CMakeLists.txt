cmake_minimum_required(VERSION 3.5)

project(libtifiles2
        VERSION     1.1.8
        LANGUAGES   C)

add_definitions(-DPACKAGE="${PROJECT_NAME}" -DVERSION="${PROJECT_VERSION}" -DTIFILES_EXPORTS)

pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive ticonv)
add_definitions(${DEPS_CFLAGS})
link_directories(${DEPS_LIBRARY_DIRS})
link_libraries(${DEPS_LIBRARIES})

include_directories(src)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/cert.c
        src/comments.c
        src/error.c
        src/files8x.c
        src/files9x.c
        src/filesnsp.c
        src/filesxx.c
        src/filetypes.c
        src/grouped.c
        src/intelhex.c
        src/misc.c
        src/rwfile.c
        src/tifiles.c
        src/tigroup.c
        src/type2str.c
        src/types68k.c
        src/types83p.c
        src/typesnsp.c
        src/typesoldz80.c
        src/typesxx.c
        src/ve_fp.c)

add_library(tifiles2_objlib OBJECT ${SRC_FILES})
set_target_properties(tifiles2_objlib PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(tifiles2_shared SHARED $<TARGET_OBJECTS:tifiles2_objlib>)
add_library(tifiles2_static STATIC $<TARGET_OBJECTS:tifiles2_objlib>)

set_target_properties(tifiles2_shared PROPERTIES OUTPUT_NAME tifiles2)
set_target_properties(tifiles2_static PROPERTIES OUTPUT_NAME tifiles2)

install(TARGETS tifiles2_shared tifiles2_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
