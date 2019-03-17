cmake_minimum_required(VERSION 3.0)

project(libticalcs2-tests
        LANGUAGES   C CXX)

add_executable(torture_ticalcs torture_ticalcs.c)
add_executable(test_ticalcs_2 test_ticalcs_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticonv/trunk \
                     -L${CMAKE_BINARY_DIR}/libtifiles/trunk \
                     -L${CMAKE_BINARY_DIR}/libticables/trunk \
                     -L${CMAKE_BINARY_DIR}/libticalcs/trunk")

foreach(tar torture_ticalcs test_ticalcs_2)
    add_dependencies(${tar} ticalcs2_shared)

    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
        ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src
        ${CMAKE_SOURCE_DIR}/libticables/trunk/src
        ${CMAKE_SOURCE_DIR}/libticalcs/trunk/src)

    target_link_libraries(${tar} "${internal_libdir}" ticonv tifiles2 ticables2 ticalcs2 "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})
endforeach()

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk:${CMAKE_BINARY_DIR}/libtifiles/trunk:${CMAKE_BINARY_DIR}/libticables/trunk:${CMAKE_BINARY_DIR}/libticalcs/trunk")

add_custom_target(ticalcs2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticalcs
    DEPENDS torture_ticalcs
    COMMENT "Run the ticalcs2 torture tests"
    EXCLUDE_FROM_ALL
)
