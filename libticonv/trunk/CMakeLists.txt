cmake_minimum_required(VERSION 3.12)

project(libticonv
        VERSION     1.1.6
        LANGUAGES   CXX)

# Takes care of the i18n po/pot/gmo/mo files
i18n_mo_from_po_pot()

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/charset.cc
        src/filename.cc
        src/iconv.c
        src/ticonv.cc
        src/tokens.cc
        src/type2str.cc)

set(PUBLIC_HEADERS
        src/ticonv.h
        src/export4.h
        src/charset.h)

option(USE_ICONV "Use libiconv at runtime for libticonv (whether to link with the lib)" ON)

# external deps lookup
pkg_check_modules(DEPS REQUIRED glib-2.0)

try_static_libs_if_needed()

# auto-creation of all targets with flags etc., alongside with internal deps
create_targets_both_lib_types(ticonv)

set_target_properties(ticonv_shared PROPERTIES VERSION 9.0.4 SOVERSION 9)

if(USE_ICONV)
    find_package(Iconv REQUIRED)
    # flags/link for external deps
    add_compile_definitions(USE_ICONV)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(ticonv_objlib PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(ticonv_shared ${Iconv_LIBRARIES})
    endif()
endif()

# public export define
target_compile_definitions(ticonv_objlib PUBLIC TICONV_EXPORTS)

# tests
add_subdirectory(tests)
