dnl Process this file with autoconf to produce a configure script.

# Init.
AC_PREREQ(2.57)
AC_INIT(libticalcs2, 1.1.10, [tilp-users@lists.sourceforge.net])

dnl Release versioning info
VERSION="1.1.10"
AC_SUBST(VERSION)

dnl Library version.
dnl current:revision:age
dnl  * On a new release, increment 'revision'.
dnl  * If you added things to the api, increment 'current',
dnl    reset 'revision' and increment 'age'.
dnl  * If you changed/removed things from the api, increment 'current',
dnl    reset 'revision' and reset 'age'.
m4_define(LT_CURRENT, 13)
m4_define(LT_REVISION, 3)
m4_define(LT_AGE, 0)
LT_LIBVERSION="LT_CURRENT():LT_REVISION():LT_AGE()"
AC_SUBST(LT_LIBVERSION)

AM_INIT_AUTOMAKE([dist-bzip2 subdir-objects])
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

AC_PREFIX_DEFAULT(/usr/local)

dnl Files to configure.
AC_CONFIG_SRCDIR([src/ticalcs.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  build/mingw/Makefile
  docs/Makefile
  po/Makefile.in
  src/Makefile
  src/romdump_8x/Makefile
  src/romdump_84p_usb/Makefile
  src/romdump_9x/Makefile
  src/romdump_834pce_usb/Makefile
  tests/Makefile
  ticalcs2.pc
])

# Setup libtool.
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_AWK
AC_CHECK_PROG(TFDOCGEN, tfdocgen, yes, no)
AM_CONDITIONAL(USE_DOCGEN, test "$TFDOCGEN" != "no")
AC_CHECK_TOOL(RC, windres, windres)

# Checks for libraries.
AM_GNU_GETTEXT(external)
AM_GNU_GETTEXT_VERSION([0.18])

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.6.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(TICABLES, ticables2 >= 1.3.5)
AC_SUBST(TICABLES_CFLAGS)
AC_SUBST(TICABLES_LIBS)

PKG_CHECK_MODULES(TICONV, ticonv >= 1.1.5)
AC_SUBST(TICONV_CFLAGS)
AC_SUBST(TICONV_LIBS)

PKG_CHECK_MODULES(TIFILES, tifiles2 >= 1.1.0)
AC_SUBST(TIFILES_CFLAGS)
AC_SUBST(TIFILES_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_C_RESTRICT

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS([bzero memmove memset strcasecmp strdup localtime_r asctime_r])

# Platform specific tests.
dnl AC_CANONICAL_HOST
case "$host" in
  *-*-*bsd*)  ARCH="-D__BSD__" ;;
  *-*-mingw*) ARCH="-D__WIN32__ -D__MINGW32__" ;;
  *)          ARCH="-D__LINUX__" ;;
esac
CFLAGS="$CFLAGS $ARCH"
CXXFLAGS="$CXXFLAGS $ARCH"

# Ensure MSVC-compatible struct packing convention is used when
# compiling for Win32 with gcc.
# What flag to depends on gcc version: gcc3 uses "-mms-bitfields", while
# gcc2 uses "-fnative-struct".
case $host_os in
*mingw*|*cygwin*)
  if test x$GCC = xyes; then
    msnative_struct=''
    AC_MSG_CHECKING([how to get MSVC-compatible struct packing])
    if test -z "$ac_cv_prog_CC"; then
      our_gcc="$CC"
    else
      our_gcc="$ac_cv_prog_CC"
    fi
    case `$our_gcc --version | sed -e 's,\..*,.,' -e q` in
      2.)
        if $our_gcc -v --help 2>/dev/null | grep fnative-struct >/dev/null; then
          msnative_struct='-fnative-struct'
        fi
        ;;
      *)
        if $our_gcc -v --help 2>/dev/null | grep ms-bitfields >/dev/null; then
          msnative_struct='-mms-bitfields'
        fi
        ;;
    esac
    if test x"$msnative_struct" = x ; then
      AC_MSG_RESULT([no way])
      AC_MSG_WARN([produced libraries might be incompatible with MSVC-compiled code])
    else
      CFLAGS="$CFLAGS $msnative_struct"
      CXXFLAGS="$CXXFLAGS $msnative_struct"
      AC_MSG_RESULT([${msnative_struct}])
    fi
  fi
  ;;
esac

AM_CONDITIONAL(OS_WIN32, test "$msnative_struct")

# Check for the new -fvisibility=hidden flag introduced in gcc 4.0
# Allow to reduce shared library size and avoid symbol clash
case $host_os in
*mingw*)
  ;;
*)
	if test x$GCC = xyes; then
	   visibility_flag=''
	   AC_MSG_CHECKING([whether gcc accepts -fvisibility])
	   if test -z "$ac_cv_prog_CC"; then
	      our_gcc="$CC"
	   else
	      our_gcc="$ac_cv_prog_CC"
	   fi
	   if $our_gcc -v --help 2>/dev/null | grep "fvisibility" >/dev/null; then
	      visibility_flag='-fvisibility=hidden'
	   fi
	   if test x"$visibility_flag" = x ; then
	      AC_MSG_RESULT([no])
	   else
	      CFLAGS="$CFLAGS $visibility_flag"
	      CXXFLAGS="$CXXFLAGS $visibility_flag"
	      AC_MSG_RESULT([${visibility_flag}])
	      AC_DEFINE(HAVE_FVISIBILITY, 1, [Use -fvisibility=hidden flag])
	    fi
	fi
	;;
esac

# Check for tools needed to build ROM dumpers

# Find a C++ compiler for the build system (needed for compiling tf2hex)
AC_ARG_VAR([CXX_FOR_BUILD], [C++ compiler for programs to be run on the build system])
AC_ARG_VAR([CXXFLAGS_FOR_BUILD], [C++ compiler flags for CXX_FOR_BUILD])
AC_MSG_CHECKING([for the host compiler])
if test "x$build" = "x$host" ; then
  # Not cross-compiling.
  CXX_FOR_BUILD=$CXX
  CXXFLAGS_FOR_BUILD=$CXXFLAGS
  AC_MSG_RESULT([${CXX_FOR_BUILD}])
else
  # Try to find the host CXX among several choices
  AC_CHECK_PROGS(CXX_FOR_BUILD, [c++ g++ clang++], [false])
  if test "x$CXX_FOR_BUILD" = "xfalse" ; then
    AC_MSG_FAILURE([not found.])
  else
    AC_MSG_RESULT([${CXX_FOR_BUILD}])
  fi
fi

# * Pass --disable-builtin-rom-dumpers to force the ROM dumpers to be
#   recompiled. The default is not to recompile them (but maintainers should
#   recompile them and watch out for changes !)

AC_ARG_ENABLE([builtin-rom-dumpers],
  AS_HELP_STRING([--disable-builtin-rom-dumpers], [do recompile ROM dumping utilities]),
  [], [enable_builtin_rom_dumpers=yes])

AC_ARG_VAR([SPASM], [SPASM Z80 assembler])
AC_ARG_VAR([TIFILEUTIL], [Tool for creating TI variable files])
have_spasm=no
have_tifileutil=no
have_tigcc=no
have_m68klinuxgnu=no
: ${SPASM=spasm}
: ${TIFILEUTIL=tifileutil}

if test "x$enable_builtin_rom_dumpers" != "xyes" ; then
  AC_MSG_CHECKING([whether $SPASM works])
  cat >conftest.asm <<EOF
    .addinstr B_CALL * EF 3 NOP 1
    .org 9d95h
    B_CALL 4972h
    ret
EOF
  if "$SPASM" conftest.asm conftest.bin >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD ; then
    have_spasm=yes
  fi
  AC_MSG_RESULT([$have_spasm])

  AC_MSG_CHECKING([whether $TIFILEUTIL works])
  if "$TIFILEUTIL" wrap conftest.bin -p -o conftest.8xp >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD ; then
    have_tifileutil=yes
  fi
  AC_MSG_RESULT([$have_tifileutil])

  AC_MSG_CHECKING([whether tigcc works])
  cat >conftest.c <<EOF
#define USE_TI89
#define USE_TI92P
#define USE_V200

#define MIN_AMS 100
#define NO_AMS_CHECK
#define SAVE_SCREEN

#include <tigcclib.h>

void _main(void) {
    ClrScr();
    DrawStr(0, 0, "Hello World !", A_NORMAL);
    GKeyIn(NULL, 0);
}
EOF
  if tigcc -Os -Wall -W -v conftest.c -o conftest >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD ; then
    have_tigcc=yes
  fi
  AC_MSG_RESULT([$have_tigcc])

  AC_MSG_CHECKING([whether m68k-linux-gnu toolchain works])
  if m68k-linux-gnu-as - < /dev/null >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD ; then
    if m68k-linux-gnu-ld -v >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD ; then
      have_m68klinuxgnu=yes
    fi
  fi
  AC_MSG_RESULT([$have_m68klinuxgnu])
fi

if test "x$have_spasm$have_tifileutil" = "xyesyes" ; then
  build_z80_rom_dumpers=yes
else
  build_z80_rom_dumpers=no
  if test "x$enable_builtin_rom_dumpers" = "xno" ; then
    AC_MSG_FAILURE([spasm and/or tifileutil are not installed or are not usable.])
  fi
fi

if test "x$have_tigcc" = "xyes" -o "x$have_m68klinuxgnu" = "xyes" ; then
  build_68k_rom_dumpers=yes
else
  build_68k_rom_dumpers=no
  if test "x$enable_builtin_rom_dumpers" = "xno" ; then
    AC_MSG_FAILURE([tigcc or m68k-linux-gnu toolchain is not installed or is not usable.])
  fi
fi

AM_CONDITIONAL(BUILD_Z80_ROM_DUMPERS, test "x$build_z80_rom_dumpers" = "xyes")
AM_CONDITIONAL(BUILD_68K_ROM_DUMPERS, test "x$build_68k_rom_dumpers" = "xyes")
AM_CONDITIONAL(BUILD_68K_ROM_DUMPERS_USING_TIGCC, test "x$build_68k_rom_dumpers" = "xyes" -a "x$have_tigcc" = "xyes")

# Output.
AC_OUTPUT

echo "Now, you can type 'make' and 'make install'."
