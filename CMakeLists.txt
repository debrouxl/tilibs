cmake_minimum_required(VERSION 3.9)

project(tilibs C CXX)

#########################################
# Experimental CMake support for tilibs #
#########################################
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
# In the future...:
#   - TODO: add support to build the ROM dumpers (will end up just launching the external tools...)
#   - WISH: do not hardcode the (auto-generated...) potfiles_* target names
#   - WISH: better expose each lib's generated install target (NTS: look at cmake's components feature?)
#   - WISH: provide Find* CMake modules for the libs themselves?
#   - WISH: add support for Windows (that will probably be annoying)
#

# Our modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.cmake)
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/*.cmake")
foreach(file ${files})
    include(${file})
endforeach()

set(USED_CMAKE_GENERATOR "${CMAKE_GENERATOR}" CACHE STRING "Expose CMAKE_GENERATOR" FORCE)

message(STATUS "Detected system: ${CMAKE_SYSTEM_NAME} - host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

option(TRY_STATIC_LIBS "Build using as many static libs as possible" OFF)
if(TRY_STATIC_LIBS)
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()
endif()

include(GNUInstallDirs)
include(CheckSymbolExists)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLOBAL_COMPILE_FLAGS "-W -Wall -Wextra -Wno-unused-parameter -Werror=shadow -Werror=write-strings -Wredundant-decls -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=date-time -Werror=missing-prototypes -Werror=return-type -Werror=pointer-arith ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLOBAL_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_COMPILE_FLAGS}")

if(APPLE)
    add_definitions(-D__MACOSX__)
elseif(UNIX)
    add_definitions(-D__LINUX__)
elseif(MINGW)
    add_definitions(-D__MINGW32__)
elseif(WIN32)
    add_definitions(-D__WIN32__)
endif()

# Needed for C++ compat
add_definitions(-Drestrict=__restrict)

# i18n support checking
include(FindGettext)
include(FindIntl)
if(Intl_FOUND AND GETTEXT_FOUND)
    set(ENABLE_NLS 1)
    add_definitions(-DENABLE_NLS=1)
    if(TRY_STATIC_LIBS)
        # needed by __nl_find_msg in libintl.a(dcigettext.o)...
        find_package(Iconv REQUIRED)
    endif()
else()
    message(WARNING "The Intl and GetText libs are needed for translations - Only English will be available")
endif()
set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}")

# Global defines
add_definitions(-DHAVE_FVISIBILITY=1 -DLOCALEDIR="${LOCALEDIR}")
check_symbol_exists(ctime_r "time.h" HAVE_CTIME_R)
check_symbol_exists(localtime_r "time.h" HAVE_LOCALTIME_R)
check_symbol_exists(asctime_r "time.h" HAVE_ASCTIME_R)
if(HAVE_CTIME_R)
add_definitions(-DHAVE_CTIME_R=1)
endif()
if(HAVE_LOCALTIME_R)
add_definitions(-DHAVE_LOCALTIME_R=1)
endif()
if(HAVE_ASCTIME_R)
add_definitions(-DHAVE_ASCTIME_R=1)
endif()

# For libs finding
find_package(PkgConfig)

add_subdirectory(libticonv/trunk)
add_subdirectory(libtifiles/trunk)
add_subdirectory(libticables/trunk)
add_subdirectory(libticalcs/trunk)

add_custom_target(check
    DEPENDS ticonv_check tifiles2_check ticables2_check ticalcs2_check
    COMMENT "Run all torture tests"
    EXCLUDE_FROM_ALL
)
