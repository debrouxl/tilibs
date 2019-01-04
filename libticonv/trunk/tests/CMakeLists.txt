cmake_minimum_required(VERSION 3.0)

project(libticonv-tests
        LANGUAGES   C)

add_executable(torture_ticonv torture_ticonv.c)
add_dependencies(torture_ticonv ticonv_shared)

pkg_check_modules(DEPS REQUIRED glib-2.0)

target_compile_options(torture_ticonv PRIVATE ${DEPS_CFLAGS})

target_include_directories(torture_ticonv PRIVATE
    ${CMAKE_SOURCE_DIR}/libticonv/trunk/src)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticonv/trunk")

target_link_libraries(torture_ticonv "${internal_libdir}" ticonv "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk")

add_custom_target(ticonv_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticonv
    DEPENDS torture_ticonv
    COMMENT "Run the ticonv torture tests"
    EXCLUDE_FROM_ALL
)
