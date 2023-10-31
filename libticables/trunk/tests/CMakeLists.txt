cmake_minimum_required(VERSION 3.12)

project(libticables2-tests
        LANGUAGES   C CXX)

add_executable(torture_ticables torture_ticables.c)
add_executable(test_ticables_2 test_ticables_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0>=1.0.16)

foreach(tar torture_ticables test_ticables_2)
    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${CMAKE_SOURCE_DIR}/libticables/trunk/src)

    target_link_directories(${tar} PRIVATE
        ${DEPS_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR}/libticables/trunk)

    target_link_libraries(${tar} ${DEPS_LIBRARIES} ticables2_shared)
endforeach()

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticables/trunk")

add_custom_target(ticables2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticables
    DEPENDS torture_ticables
    COMMENT "Run the ticables2 torture tests"
    EXCLUDE_FROM_ALL
)
