cmake_minimum_required(VERSION 3.12)

project(tilibs C CXX)

############################
# CMake support for tilibs #
############################
#
# This aims to provide a much simpler way to build and install libti* on
# computers that have decent and recent OS and toolchains.
# As a bonus, it provides better support for CMake-oriented IDE (e.g. CLion).
#
# Features:
#   - builds and installs both shared and static libraries
#   - installs the public headers of each library
#   - creates and installs i18n .mo files (when libs/tools are present)
#   - configures and installs the .pc files for pkg-config
#
# Caveats:
#   - no support for the ROM dumpers subdirectories
#   - not all the autotools/configure options are available
#   - no updatepot/updatepo targets are available, but simply executing intltool-update does the job...
#   - probably doesn't work very well with ancient toolchains or on uncommon OSes
#
# Notes:
#   - this has been succesfully tested on recent macOS, Linux, and Windows (with vcpkg).
#   - The TRY_STATIC_LIBS CMake option will try to build the libs (both shared and static) using as many static
#     dependent libraries as possible.
#     Let's note however that there is currently not much advantage in building that way, using shared libs is preferred,
#     but at least you have the choice to do whatever you want should you ever really need a static build (may be useful on Windows)
#
# In the future...:
#   - TODO: add support to build the ROM dumpers (will end up just launching the external tools...)
#   - WISH: do not hardcode the (auto-generated...) potfiles_* target names
#   - WISH: better expose each lib's generated install target (NTS: look at cmake's components feature?)
#   - WISH: provide Find* CMake modules for the libs themselves?
#

# In case there was an env-var override...
if(DEFINED ENV{CMAKE_INSTALL_PREFIX_OVERRIDE})
    set(CMAKE_INSTALL_PREFIX "$ENV{CMAKE_INSTALL_PREFIX_OVERRIDE}")
endif()
# Be sure to have the expected install prefix format
if(CMAKE_INSTALL_PREFIX)
    file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "CMAKE_INSTALL_PREFIX" FORCE)
    message("CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
endif()

# Our modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.cmake)
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/*.cmake")
foreach(file ${files})
    include(${file})
endforeach()

set(USED_CMAKE_GENERATOR "${CMAKE_GENERATOR}" CACHE STRING "Expose CMAKE_GENERATOR" FORCE)
message(STATUS "Detected system: ${CMAKE_SYSTEM_NAME} - host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR} - CXX_COMPILER: ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if(supported)
        message(STATUS "IPO/LTO enabled")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO/LTO not supported: <${error}>")
    endif()
else()
    message("IPO/LTO not enabled because this is not a release build")
endif()

option(TRY_STATIC_LIBS "Build using as many static libs as possible" OFF)
if (VCPKG_TARGET_TRIPLET MATCHES "-static")
    if(NOT TRY_STATIC_LIBS)
        message(WARNING "Setting TRY_STATIC_LIBS to ON anyway due to static vcpkg target")
        set(TRY_STATIC_LIBS ON)
    endif()
endif()

if(TRY_STATIC_LIBS)
    message("Will try to build as statically as possible...")
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()
    # When we link statically, we don't want the dllimport stuff, so let's workaround that here
    add_compile_definitions(TICALCS_EXPORTS TIFILES_EXPORTS TICONV_EXPORTS TICABLES_EXPORTS)
endif()

include(GNUInstallDirs)
include(CheckSymbolExists)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(APPLE)
    add_compile_definitions(__MACOSX__)
elseif(UNIX)
    add_compile_definitions(__LINUX__)
    set(LINUX TRUE)
elseif(MINGW)
    add_compile_definitions(__MINGW32__)
elseif(WIN32)
    add_compile_definitions(__WIN32__)
    add_compile_definitions(HAVE_CONFIG_H)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    include_directories(${CMAKE_SOURCE_DIR}/win32_config)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # sane defaults + hardening
    set(GLOBAL_COMPILE_FLAGS "-W -Wall -Wextra -Wno-unused-parameter -Werror=shadow -Werror=write-strings -Wredundant-decls -Werror=date-time -Werror=return-type -Werror=pointer-arith")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLOBAL_COMPILE_FLAGS} -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=missing-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_COMPILE_FLAGS}")
    if(LINUX)
        add_link_options("-Wl,-z,relro,-z,now -Wl,-z,noexecstack -Wl,-z,separate-code")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            add_link_options("-Wl,--as-needed")
        endif()
    endif()
    # useful flags for debugging
    set(GLOBAL_DEBUG_FLAGS "-fno-omit-frame-pointer -fsanitize=address,bounds -fsanitize-undefined-trap-on-error -fstack-protector-strong -fstack-clash-protection")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
    # only for clang/gcc
    add_compile_definitions(HAVE_FVISIBILITY=1)
elseif (MSVC)
    # sane defaults + hardening. /wd4996 to disable deprecated functions warnings
    set(GLOBAL_COMPILE_FLAGS "/Wall /sdl /guard:cf /guard:ehcont /wd4996")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLOBAL_COMPILE_FLAGS} ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_COMPILE_FLAGS} ")
    # useful flags for debugging
    set(GLOBAL_DEBUG_FLAGS "/fsanitize=address")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GLOBAL_DEBUG_FLAGS} ")
endif()

# i18n support checking
include(FindGettext)
include(FindIntl)
if(Intl_FOUND AND GETTEXT_FOUND)
    set(ENABLE_NLS 1)
    include_directories(${Intl_INCLUDE_DIR})
    add_compile_definitions(ENABLE_NLS=1)
    if(TRY_STATIC_LIBS)
        # needed by __nl_find_msg in libintl.a(dcigettext.o)...
        find_package(Iconv REQUIRED)
        if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
            include_directories(${Iconv_INCLUDE_DIRS})
            if(Iconv_LIBRARY_DIRS)
                link_directories(${Iconv_LIBRARY_DIRS})
            endif()
            link_libraries(${Iconv_LIBRARIES})
        endif()
    else()
        link_directories(${Intl_LIBRARY_DIRS})
        link_libraries(${Intl_LIBRARIES})
    endif()
else()
    message(WARNING "The Intl and GetText libs are needed for translations - Only English will be available")
endif()
set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}")

# Global defines
add_compile_definitions(LOCALEDIR="${LOCALEDIR}")
check_symbol_exists(ctime_r "time.h" HAVE_CTIME_R)
check_symbol_exists(localtime_r "time.h" HAVE_LOCALTIME_R)
check_symbol_exists(asctime_r "time.h" HAVE_ASCTIME_R)
if(HAVE_CTIME_R)
    add_compile_definitions(HAVE_CTIME_R=1)
endif()
if(HAVE_LOCALTIME_R)
    add_compile_definitions(HAVE_LOCALTIME_R=1)
endif()
if(HAVE_ASCTIME_R)
    add_compile_definitions(HAVE_ASCTIME_R=1)
endif()

# For libs finding
find_package(PkgConfig)

# Set manually when installed with Homebrew, see https://github.com/Homebrew/legacy-homebrew/issues/45891
# Note: we can't do the same for libiconv because it exports libiconv_* symbols and not iconv_*, so we fallback on the non-static system lib...
if(APPLE)
    set(BREW_LIB_PATHS "/opt/homebrew/lib" "/usr/local/opt" "/usr/local/lib")
    foreach(LIB_DIR ${BREW_LIB_PATHS})
        if(EXISTS "${LIB_DIR}/libarchive/lib/pkgconfig")
            set(ENV{PKG_CONFIG_PATH} "${LIB_DIR}/libarchive/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
            break()
        endif()
    endforeach()
endif()

add_subdirectory(libticonv/trunk)
add_subdirectory(libtifiles/trunk)
add_subdirectory(libticables/trunk)
add_subdirectory(libticalcs/trunk)

add_custom_target(check
    DEPENDS ticonv_check tifiles2_check ticables2_check ticalcs2_check
    COMMENT "Run all torture tests"
    EXCLUDE_FROM_ALL
)
