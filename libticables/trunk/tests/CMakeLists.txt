cmake_minimum_required(VERSION 3.12)

project(libticables2-tests
        LANGUAGES   C CXX)

add_executable(torture_ticables torture_ticables.c)
add_executable(test_ticables_2 test_ticables_2.cc)

if(WIN32)
    pkg_check_modules(DEPS REQUIRED glib-2.0)
    list(APPEND DEPS_LIBRARIES "libusb0")
    list(APPEND DEPS_STATIC_LIBRARIES "libusb0")
else()
    pkg_check_modules(DEPS REQUIRED glib-2.0 libusb-1.0>=1.0.16)
endif()

foreach(tar torture_ticables test_ticables_2)
    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../src)

    if(TRY_STATIC_LIBS)
        target_compile_options(${tar} PRIVATE ${DEPS_STATIC_CFLAGS})
        target_link_directories(${tar} PRIVATE ${TRY_STATIC_DEPS_LIBSDIRS})
        target_link_libraries(${tar} ${TRY_STATIC_DEPS_LDFLAGS_OTHER} ${TRY_STATIC_DEPS_LIBS} ticables2_objlib)
    else()
        target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})
        target_link_directories(${tar} PRIVATE ${DEPS_LIBRARY_DIRS} ${PROJECT_BINARY_DIR}/..)
        target_link_libraries(${tar} ${DEPS_LIBRARIES} ticables2_objlib)
    endif()
endforeach()

set(builddirlibpaths "${PROJECT_BINARY_DIR}")

add_custom_target(ticables2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticables
    DEPENDS torture_ticables
    COMMENT "Run the ticables2 torture tests"
    EXCLUDE_FROM_ALL
)
