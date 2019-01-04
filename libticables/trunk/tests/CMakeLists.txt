cmake_minimum_required(VERSION 3.0)

project(libticables2-tests
        LANGUAGES   C)

add_executable(torture_ticables torture_ticables.c)
add_dependencies(torture_ticables ticables2_shared)

pkg_check_modules(DEPS REQUIRED glib-2.0)

target_compile_options(torture_ticables PRIVATE ${DEPS_CFLAGS})

target_include_directories(torture_ticables PRIVATE
    ${CMAKE_SOURCE_DIR}/libticables/trunk/src)

set(internal_libdir "-L${CMAKE_BINARY_DIR}/libticables/trunk")

target_link_libraries(torture_ticables "${internal_libdir}" ticables2 "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES})

set(builddirlibpaths "${CMAKE_BINARY_DIR}/libticables/trunk")

add_custom_target(ticables2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticables
    DEPENDS torture_ticables
    COMMENT "Run the ticables2 torture tests"
    EXCLUDE_FROM_ALL
)
