dnl Process this file with autoconf to produce a configure script.

# Init.
AC_PREREQ(2.57)
AC_INIT(libticables2, 1.3.6, [tilp-users@lists.sourceforge.net])

dnl Release versioning info
VERSION="1.3.6"
AC_SUBST(VERSION)

dnl Library version.
dnl current:revision:age
dnl  * On a new release, increment 'revision'.
dnl  * If you added things to the api, increment 'current',
dnl    reset 'revision' and increment 'age'.
dnl  * If you changed/removed things from the api, increment 'current',
dnl    reset 'revision' and reset 'age'.
m4_define(LT_CURRENT, 8)
m4_define(LT_REVISION, 0)
m4_define(LT_AGE, 0)
LT_LIBVERSION="LT_CURRENT():LT_REVISION():LT_AGE()"
AC_SUBST(LT_LIBVERSION)

AM_INIT_AUTOMAKE([dist-bzip2 subdir-objects])
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

AC_PREFIX_DEFAULT(/usr/local)

dnl Files to configure.
AC_CONFIG_SRCDIR([src/ticables.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  build/mingw/Makefile
  docs/Makefile
  po/Makefile.in
  src/Makefile
  src/win32/dha/Makefile
  src/win64/rwp/Makefile
  tests/Makefile
  ticables2.pc
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

# Call explicitely before using PKG_*
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.4.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

# USB cable support
AC_ARG_ENABLE(libusb,
  AC_HELP_STRING([--disable-libusb], [disable USB support through libusb 0.1.x / 1.0.x]),
  [cab_cv_use_libusb=$enableval], [cab_cv_use_libusb=yes])
AC_CACHE_CHECK([whether to enable USB through outdated libusb 0.1.x],
  [cab_cv_use_libusb], [cab_cv_use_libusb=no])

AC_ARG_ENABLE(libusb10,
  AC_HELP_STRING([--enable-libusb10], [enable USB support through libusb 1.0.x]),
  [cab_cv_use_libusb10=$enableval], [cab_cv_use_libusb10=no])
AC_CACHE_CHECK([whether to enable USB through libusb 1.0.x],
  [cab_cv_use_libusb10], [cab_cv_use_libusb10=no])

if [test "x$cab_cv_use_libusb" != xno]; then
  if [test "x$cab_cv_use_libusb10" != xno]; then
    PKG_CHECK_MODULES([LIBUSB10], [ libusb-1.0 >= 1.0.0 ],
      [HAVE_LIBUSB10=yes
       LIBS="$LIBUSB10_LIBS"
       CFLAGS="$LIBUSB10_CFLAGS"
       AC_TRY_LINK([#include <libusb.h>], [  const char *s = libusb_strerror(0)],
                    [AC_DEFINE([HAVE_LIBUSB10_STRERROR], [1], [libusb 1.0.x has libusb_strerror])],
                    [AC_ERROR([Use a version of libusb which provides libusb_strerror])])
      ])
  else
    case $host_os in
    *mingw*|*cygwin*)
      # use builtin libusb_dyn.c to load libusb at runtime
      HAVE_LIBUSB=yes
      LIBUSB_CFLAGS='-I$(top_srcdir)/src/win32'
      LIBUSB_LIBS=
      ;;

    *)
      PKG_CHECK_MODULES([LIBUSB], [ libusb >= 0.1.12 ],
        [HAVE_LIBUSB=yes])
      ;;
    esac
  fi
fi

if test "x$HAVE_LIBUSB10" = xyes; then
  AC_DEFINE([HAVE_LIBUSB_1_0], [1],
            [Define to 1 to enable EXPERIMENTAL libusb 1.0.x support.])
  TICABLES_LIBUSB_REQUIRES_PRIVATE="libusb-1.0"
elif test "x$HAVE_LIBUSB" = xyes; then
  case $host_os in
  *mingw*|*cygwin*)
    ;;
  *)
    AC_DEFINE([HAVE_LIBUSB], [1], [Define to 1 to enable libusb 0.1.x support.])
    TICABLES_LIBUSB_REQUIRES_PRIVATE="libusb"
    ;;
  esac
fi
AC_SUBST(TICABLES_LIBUSB_REQUIRES_PRIVATE)

AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)
AC_SUBST(LIBUSB10_CFLAGS)
AC_SUBST(LIBUSB10_LIBS)

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h termios.h linux/parport.h linux/ppdev.h linux/serial.h] linux/ticable.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_C_CONST
AC_C_RESTRICT
AC_HEADER_TIME

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_CHECK_FUNCS([cfmakeraw])

# Platform specific tests.
dnl AC_CANONICAL_HOST
case "$host" in
  i[[3456]]86-*-*bsd*)   ARCH="-D__BSD__ -D__I386__" ;;
  *-*-*bsd*)             ARCH="-D__BSD__" ;;
  aarch64-apple-darwin*) ARCH="-D__MACOSX__" ;;
  aarch64-*-linux-*)     ARCH="-D__LINUX__" ;;
  alpha*-*-linux-*)      ARCH="-D__ALPHA__ -D__LINUX__" ;;
  alpha*-*-*-*)          ARCH="-D__ALPHA__ -D__LINUX__" ;;
  arm*-*-linux-*)        ARCH="-D__ARM__ -D__LINUX__" ;;
  hppa*)                 ARCH="-D__LINUX__" ;;
  i[[3456]]86-*-linux-*) ARCH="-D__I386__ -D__LINUX__" ;;
  i[[3456]]86-*-hurd-*)  ARCH="-D__I386__ -D__LINUX__" ;;
  i[[3456]]86-*-gnu-*)   ARCH="-D__I386__ -D__LINUX__" ;;
  i[[3456]]86-*-k*bsd-gnu*-*) ARCH="-D__I386__ -D__LINUX__" ;;
  k*bsd-gnu*|gnu*)       ARCH="-D__LINUX__" ;;
  i[[3456]]86-*-mingw*)  ARCH="-D__WIN32__ -D__MINGW32__" ;;
  i[[3456]]86-apple-darwin*) ARCH="-D__I386__ -D__MACOSX__" ;;
  x86_64-*-linux-*)      ARCH="-D__LINUX__" ;;
  x86_64-apple-darwin*)  ARCH="-D__MACOSX__" ;;
  ia64-*-linux-*)        ARCH="-D__LINUX__" ;;
  loongarch64-*-linux-*) ARCH="-D__LOONGARCH__ -D__LINUX__" ;;
  m68k-*-linux-*)        ARCH="-D__M68K__ -D__LINUX__" ;;
  mips*-*-linux-*)       ARCH="-D__MIPS__ -D__LINUX__" ;;
  powerpc-*-linux-*)     ARCH="-D__PPC__  -D__LINUX__" ;;
  powerpc64-*-linux-*)   ARCH="-D__PPC__  -D__LINUX__" ;;
  powerpc64le-*-linux-*) ARCH="-D__PPC__  -D__LINUX__" ;;
  powerpc-apple-darwin*) ARCH="-D__PPC__  -D__MACOSX__" ;;
  powerpc64-apple-darwin*) ARCH="-D__PPC__  -D__MACOSX__" ;;
  riscv64-*-linux-*)     ARCH="-D__RISCV__ -D__LINUX__" ;;
  s390*-*-linux-*)       ARCH="-D__LINUX__" ;;
  sh*-*-linux-*)         ARCH="-D__LINUX__" ;;
  sparc*-*-linux-*)      ARCH="-D__SPARC__ -D__LINUX__" ;;
  sparc*-sun-solaris*)   ARCH="-D__SPARC__ -D__SOL__" ;;
  *)                     ARCH="ERROR" ;;
esac
if test "x$ARCH" = xERROR; then
   AC_ERROR([Unhandled target triple, please open a bug report mentioning "$host"])
fi
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

# Logging
AC_ARG_ENABLE(logging,
  AC_HELP_STRING([--enable-logging], [enable logging]),
  [cab_cv_enable_logging=$enableval], [])
AC_CACHE_CHECK([whether to enable logging],
  [cab_cv_enable_logging], [cab_cv_enable_logging=no])
if test x$cab_cv_enable_logging = xyes; then
  AC_DEFINE(ENABLE_LOGGING, 1, [Define to 1 to enable logging.])
fi

# Output.
AC_OUTPUT

echo "Now, you can type 'make' and 'make install'."

if test x$cab_cv_enable_logging = xyes; then
 echo ""
 echo "Logging has been enabled. Log files are generated in a user's"
 echo "home directory."
 echo ""
else
 echo ""
 echo "Developpers or users who want to send a bug report can add the" 
 echo "'--enable-logging' option on the 'configure' command line to" 
 echo "enable logging. Log files are generated in a user's home directory."
 echo ""
fi
