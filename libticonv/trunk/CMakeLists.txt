cmake_minimum_required(VERSION 3.5)

project(libticonv
        VERSION     1.1.6
        LANGUAGES   C)

add_definitions(-DPACKAGE="${PROJECT_NAME}" -DVERSION="${PROJECT_VERSION}" -DTICONV_EXPORTS)

find_package(Iconv REQUIRED)
if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
    link_libraries(${Iconv_LIBRARIES})
endif()

configure_and_install_pc_file(ticonv ${PROJECT_VERSION})

pkg_check_modules(DEPS REQUIRED glib-2.0)
add_definitions(${DEPS_CFLAGS})
link_directories(${DEPS_LIBRARY_DIRS})
link_libraries(${DEPS_LIBRARIES} ${Intl_LIBRARIES})

include_directories(src)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/charset.c
        src/filename.c
        src/iconv.c
        src/ticonv.c
        src/tokens.c
        src/type2str.c)

set(PUBLIC_HEADERS
        src/ticonv.h
        src/export4.h
        src/charset.h)

add_library(ticonv_objlib OBJECT ${SRC_FILES})
set_target_properties(ticonv_objlib PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(ticonv_shared SHARED $<TARGET_OBJECTS:ticonv_objlib>)
add_library(ticonv_static STATIC $<TARGET_OBJECTS:ticonv_objlib>)

set_target_properties(ticonv_shared ticonv_static PROPERTIES
        OUTPUT_NAME     ticonv
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")

install(TARGETS ticonv_shared ticonv_static
        ARCHIVE         DESTINATION lib
        LIBRARY         DESTINATION lib
        RUNTIME         DESTINATION bin
        PUBLIC_HEADER   DESTINATION include/tilp2)

i18n_mo_from_po_pot()

create_buildandinstall_target(${PROJECT_NAME} ticonv_shared ticonv_static)
