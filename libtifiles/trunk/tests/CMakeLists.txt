cmake_minimum_required(VERSION 3.9)

project(libtifiles2-tests
        LANGUAGES   C CXX)

add_executable(torture_tifiles torture_tifiles.c)
add_executable(test_tifiles_2 test_tifiles_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticonv/trunk \
                     -L${CMAKE_BINARY_DIR}/libtifiles/trunk")

foreach(tar torture_tifiles test_tifiles_2)
    add_dependencies(${tar} tifiles2_shared)

    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
        ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src)

    target_link_libraries(${tar} "${internal_libdir}" ticonv tifiles2 "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})
endforeach()

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk:${CMAKE_BINARY_DIR}/libtifiles/trunk")

add_custom_target(tifiles2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_tifiles
    DEPENDS torture_tifiles
    COMMENT "Run the tifiles2 torture tests"
    EXCLUDE_FROM_ALL
)
