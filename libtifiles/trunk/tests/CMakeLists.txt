cmake_minimum_required(VERSION 3.12)

project(libtifiles2-tests
        LANGUAGES   C CXX)

add_executable(torture_tifiles torture_tifiles.c)
add_executable(test_tifiles_2 test_tifiles_2.cc)

pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive zlib)
if(USE_ICONV)
    find_package(Iconv REQUIRED)
    # flags/link for external deps
    add_compile_definitions(USE_ICONV)
endif()

foreach(tar torture_tifiles test_tifiles_2)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(${tar} PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(${tar} ${Iconv_LIBRARIES})
    endif()

    target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../../../libticonv/trunk/src
        ${PROJECT_SOURCE_DIR}/../src)

    if(TRY_STATIC_LIBS)
        target_compile_options(${tar} PRIVATE ${DEPS_STATIC_CFLAGS})
        target_link_directories(${tar} PRIVATE ${TRY_STATIC_DEPS_LIBSDIRS})
        target_link_libraries(${tar} ${TRY_STATIC_DEPS_LDFLAGS} ${TRY_STATIC_DEPS_LIBS} ticonv_objlib tifiles2_objlib)
    else()
        target_compile_options(${tar} PRIVATE ${DEPS_CFLAGS})
        target_link_directories(${tar} PRIVATE ${DEPS_LIBRARY_DIRS} ${PROJECT_BINARY_DIR}/..)
        target_link_libraries(${tar} ${DEPS_LIBRARIES} ticonv_objlib tifiles2_objlib)
    endif()
endforeach()

set(builddirlibpaths "${PROJECT_BINARY_DIR}/../../../libticonv/trunk:${PROJECT_BINARY_DIR}/..")

add_custom_target(tifiles2_check
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH="${builddirlibpaths}" ${CMAKE_CURRENT_BINARY_DIR}/torture_tifiles
    DEPENDS torture_tifiles
    COMMENT "Run the tifiles2 torture tests"
    EXCLUDE_FROM_ALL
)
