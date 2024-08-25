cmake_minimum_required(VERSION 3.12)

project(libtifiles2-tests
        LANGUAGES   C CXX)

add_executable(torture_tifiles torture_tifiles.c)
add_executable(test_tifiles_2 test_tifiles_2.cc)

pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
find_package(LibArchive REQUIRED)
if(USE_ICONV)
    find_package(Iconv REQUIRED)
    add_compile_definitions(USE_ICONV)
endif()

foreach(tar torture_tifiles test_tifiles_2)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(${tar} PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(${tar} PRIVATE ${Iconv_LIBRARIES})
    endif()

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../../../libticonv/trunk/src
        ${PROJECT_SOURCE_DIR}/../src)

    target_link_directories(${tar} PRIVATE ${PROJECT_BINARY_DIR}/..)
    target_link_libraries(${tar} PRIVATE PkgConfig::glib LibArchive::LibArchive ticonv tifiles2)
endforeach()

add_custom_target(tifiles2_check
    COMMAND "$<TARGET_FILE:torture_tifiles>"
    DEPENDS torture_tifiles
    COMMENT "Run the tifiles2 torture tests"
    EXCLUDE_FROM_ALL
)
