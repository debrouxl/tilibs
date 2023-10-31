cmake_minimum_required(VERSION 3.12)

project(libticonv-tests
        LANGUAGES   C CXX)

add_executable(torture_ticonv torture_ticonv.c)
add_executable(test_ticonv test_ticonv.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0)

foreach(tar torture_ticonv test_ticonv)
    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src)

    target_link_directories(${tar} PRIVATE
        ${DEPS_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR}/libticonv/trunk)

    target_link_libraries(${tar} ${DEPS_LIBRARIES} ticonv_shared)
endforeach()

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk")

add_custom_target(ticonv_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticonv
    DEPENDS torture_ticonv
    COMMENT "Run the ticonv torture tests"
    EXCLUDE_FROM_ALL
)
