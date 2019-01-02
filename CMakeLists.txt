cmake_minimum_required(VERSION 3.5)

project(tilibs C)

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
#   - no support for the ROM dumpers and tests subdirectories
#   - not all the autotools/configure options are available
#   - no updatepot/updatepo targets are available, but simply executing intltool-update does the job...
#   - probably doesn't work very well with ancient toolchains or on uncommon OSes
#
# In the future...:
#   - TODO: add support to build the tests (should be rather straightforward)
#   - TODO: add support to build the ROM dumpers (will end up just launching the external tools...)
#   - WISH: better expose each lib's generated install target (NTS: look at cmake's components feature?)
#   - WISH: make Travis try the cmake-based build too
#   - WISH: provide Find* CMake modules for the libs themselves?
#   - WISH: add support for Windows (that will probably be annoying)
#

set(USED_CMAKE_GENERATOR "${CMAKE_GENERATOR}" CACHE STRING "Expose CMAKE_GENERATOR" FORCE)

message(STATUS "Detected system: ${CMAKE_SYSTEM_NAME} - host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra -Wno-unused-parameter -Werror=shadow -Werror=write-strings -Wredundant-decls -Werror=format -Werror=format-nonliteral -Werror=format-security -Werror=declaration-after-statement -Werror=implicit-function-declaration -Werror=date-time -Werror=missing-prototypes -Werror=return-type -Werror=pointer-arith ")

if(APPLE)
    add_definitions(-D__MACOSX__)
elseif(UNIX)
    add_definitions(-D__LINUX__)
elseif(MINGW)
    add_definitions(-D__MINGW32__)
elseif(WIN32)
    add_definitions(-D__WIN32__)
endif()

# i18n support checking
include(FindGettext)
include(FindIntl)
if(Intl_FOUND AND GETTEXT_FOUND)
    set(ENABLE_NLS 1)
    add_definitions(-DENABLE_NLS=1)
else()
    message(WARNING "The Intl and GetText libs are needed for translations - Only English will be available")
endif()
set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale")

# Global defines
add_definitions(-DENABLE_LOGGING=1 -DHAVE_FVISIBILITY=1 -DLOCALEDIR="${LOCALEDIR}")

# For libs finding
find_package(PkgConfig)

# Our modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.cmake)
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/*.cmake")
foreach(file ${files})
    include(${file})
endforeach()

add_subdirectory(libticonv/trunk)
add_subdirectory(libtifiles/trunk)
add_subdirectory(libticables/trunk)
add_subdirectory(libticalcs/trunk)
