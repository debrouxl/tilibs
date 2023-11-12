cmake_minimum_required(VERSION 3.12)

project(libticalcs2-tests
        LANGUAGES   C CXX)

add_executable(torture_ticalcs torture_ticalcs.c)
add_executable(test_ticalcs_2 test_ticalcs_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0>=1.0.16 libarchive zlib)

foreach(tar torture_ticalcs test_ticalcs_2)
    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../../../libticonv/trunk/src
        ${PROJECT_SOURCE_DIR}/../../../libtifiles/trunk/src
        ${PROJECT_SOURCE_DIR}/../../../libticables/trunk/src
        ${PROJECT_SOURCE_DIR}/../src)

    target_link_directories(${tar} PRIVATE
        ${DEPS_LIBRARY_DIRS}
        ${PROJECT_BINARY_DIR}/../../../libticonv/trunk
        ${PROJECT_BINARY_DIR}/../../../libtifiles/trunk
        ${PROJECT_BINARY_DIR}/../../../libticables/trunk
        ${PROJECT_BINARY_DIR}/..)

    target_link_libraries(${tar} ${DEPS_LIBRARIES} ticonv_shared tifiles2_shared ticables2_shared ticalcs2_shared)
endforeach()

set(builddirlibpaths "${PROJECT_BINARY_DIR}/../../../libticonv/trunk:${PROJECT_BINARY_DIR}/../../../libtifiles/trunk:${PROJECT_BINARY_DIR}/../../../libticables/trunk:${PROJECT_BINARY_DIR}/..")

add_custom_target(ticalcs2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticalcs
    DEPENDS torture_ticalcs
    COMMENT "Run the ticalcs2 torture tests"
    EXCLUDE_FROM_ALL
)
