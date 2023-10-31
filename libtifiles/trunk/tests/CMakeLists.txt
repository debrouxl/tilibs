cmake_minimum_required(VERSION 3.12)

project(libtifiles2-tests
        LANGUAGES   C CXX)

add_executable(torture_tifiles torture_tifiles.c)
add_executable(test_tifiles_2 test_tifiles_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive zlib)

foreach(tar torture_tifiles test_tifiles_2)
    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
        ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src)

    target_link_directories(${tar} PRIVATE
        ${DEPS_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR}/libticonv/trunk
        ${CMAKE_BINARY_DIR}/libtifiles/trunk)

    target_link_libraries(${tar} ${DEPS_LIBRARIES} ticonv_shared tifiles2_shared)
endforeach()

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticonv/trunk:${CMAKE_BINARY_DIR}/libtifiles/trunk")

add_custom_target(tifiles2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_tifiles
    DEPENDS torture_tifiles
    COMMENT "Run the tifiles2 torture tests"
    EXCLUDE_FROM_ALL
)
