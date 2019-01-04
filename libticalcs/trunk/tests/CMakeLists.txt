cmake_minimum_required(VERSION 3.0)

project(libticalcs2-tests
        LANGUAGES   C)

add_executable(torture_ticalcs torture_ticalcs.c)
add_dependencies(torture_ticalcs ticalcs2_shared)

pkg_check_modules(DEPS REQUIRED glib-2.0)

target_compile_options(torture_ticalcs PRIVATE ${DEPS_CFLAGS})

target_include_directories(torture_ticalcs PRIVATE
    ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
    ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src
    ${CMAKE_SOURCE_DIR}/libticables/trunk/src
    ${CMAKE_SOURCE_DIR}/libticalcs/trunk/src)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticonv/trunk \
                     -L${CMAKE_BINARY_DIR}/libtifiles/trunk \
                     -L${CMAKE_BINARY_DIR}/libticables/trunk \
                     -L${CMAKE_BINARY_DIR}/libticalcs/trunk")

target_link_libraries(torture_ticalcs "${internal_libdir}" ticonv tifiles2 ticables2 ticalcs2 "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk:${CMAKE_BINARY_DIR}/libtifiles/trunk:${CMAKE_BINARY_DIR}/libticables/trunk:${CMAKE_BINARY_DIR}/libticalcs/trunk")

add_custom_target(ticalcs2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticalcs
    DEPENDS torture_ticalcs
    COMMENT "Run the ticalcs2 torture tests"
    EXCLUDE_FROM_ALL
)
