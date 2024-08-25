cmake_minimum_required(VERSION 3.12)

project(libticonv-tests
        LANGUAGES   C CXX)

add_executable(torture_ticonv torture_ticonv.c)
add_executable(test_ticonv test_ticonv.cc)

pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
if(USE_ICONV)
    find_package(Iconv REQUIRED)
    add_compile_definitions(USE_ICONV)
endif()

foreach(tar torture_ticonv test_ticonv)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(${tar} PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(${tar} PRIVATE ${Iconv_LIBRARIES})
    endif()

    target_include_directories(${tar} PRIVATE
        ${PROJECT_SOURCE_DIR}/../src)

    target_link_directories(${tar} PRIVATE ${PROJECT_BINARY_DIR}/..)
    target_link_libraries(${tar} PRIVATE PkgConfig::glib ticonv)
endforeach()

add_custom_target(ticonv_check
    COMMAND "$<TARGET_FILE:torture_ticonv>"
    DEPENDS torture_ticonv
    COMMENT "Run the ticonv torture tests"
    EXCLUDE_FROM_ALL
)
