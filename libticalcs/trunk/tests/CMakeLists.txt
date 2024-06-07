cmake_minimum_required(VERSION 3.12)

project(libticalcs2-tests
        LANGUAGES   CXX)

add_executable(torture_ticalcs torture_ticalcs.cc)
if(NOT WIN32) # not compatible for now
    add_executable(test_ticalcs_2 test_ticalcs_2.cc)
endif()

if(WIN32)
    pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive)
    list(APPEND DEPS_LIBRARIES "libusb0")
    list(APPEND DEPS_STATIC_LIBRARIES "libusb0")
else()
    pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive libusb-1.0>=1.0.16)
    # Needed for the .pc files configured in the function below.
    set(TICABLES_LIBUSB_REQUIRES_PRIVATE "libusb-1.0")
endif()

if(WIN32 AND NOT MINGW)
    find_library(GETOPT_LIB getopt REQUIRED)
endif()

if(USE_ICONV)
    find_package(Iconv REQUIRED)
    # flags/link for external deps
    add_compile_definitions(USE_ICONV)
endif()

foreach(tar torture_ticalcs test_ticalcs_2)
    if(tar STREQUAL test_ticalcs_2 AND WIN32)
        continue() # not compatible for now
    endif ()

    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(${tar} PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(${tar} ${Iconv_LIBRARIES})
    endif()

    if(GETOPT_LIB)
        target_link_libraries(${tar} ${GETOPT_LIB})
    endif()

    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../../../libticonv/trunk/src
        ${PROJECT_SOURCE_DIR}/../../../libtifiles/trunk/src
        ${PROJECT_SOURCE_DIR}/../../../libticables/trunk/src
        ${PROJECT_SOURCE_DIR}/../src)

    if(TRY_STATIC_LIBS)
        target_compile_options(${tar} PRIVATE ${DEPS_STATIC_CFLAGS})
        target_link_directories(${tar} PRIVATE ${TRY_STATIC_DEPS_LIBSDIRS})
        target_link_libraries(${tar} ${TRY_STATIC_DEPS_LDFLAGS_OTHER} ${TRY_STATIC_DEPS_LIBS} ticonv_objlib tifiles2_objlib ticables2_objlib ticalcs2_objlib)
    else()
        target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})
        target_link_directories(${tar} PRIVATE ${DEPS_LIBRARY_DIRS} ${PROJECT_BINARY_DIR}/..)
        target_link_libraries(${tar} ${DEPS_LIBRARIES} ticonv_objlib tifiles2_objlib ticables2_objlib ticalcs2_objlib)
    endif()
endforeach()

set(builddirlibpaths "${PROJECT_BINARY_DIR}/../../../libticonv/trunk:${PROJECT_BINARY_DIR}/../../../libtifiles/trunk:${PROJECT_BINARY_DIR}/../../../libticables/trunk:${PROJECT_BINARY_DIR}/..")

add_custom_target(ticalcs2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_ticalcs
    DEPENDS torture_ticalcs
    COMMENT "Run the ticalcs2 torture tests"
    EXCLUDE_FROM_ALL
)
