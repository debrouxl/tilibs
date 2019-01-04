cmake_minimum_required(VERSION 3.0)

project(libtifiles2-tests
        LANGUAGES   C)

add_executable(torture_tifiles torture_tifiles.c)
add_dependencies(torture_tifiles tifiles2_shared)

pkg_check_modules(DEPS REQUIRED glib-2.0)

target_compile_options(torture_tifiles PRIVATE ${DEPS_CFLAGS})

target_include_directories(torture_tifiles PRIVATE
    ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
    ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticonv/trunk \
                     -L${CMAKE_BINARY_DIR}/libtifiles/trunk")

target_link_libraries(torture_tifiles "${internal_libdir}" ticonv tifiles2 "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk:${CMAKE_BINARY_DIR}/libtifiles/trunk")

add_custom_target(tifiles2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_tifiles
    DEPENDS torture_tifiles
    COMMENT "Run the tifiles2 torture tests"
    EXCLUDE_FROM_ALL
)
