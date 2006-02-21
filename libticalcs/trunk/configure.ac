dnl Process this file with autoconf to produce a configure script.
dnl $Id$

# Init.
AC_PREREQ(2.57)
AC_INIT(libticalcs2, 0.0.6, [tilp-users@lists.sourceforge.net])

dnl Release versioning info
VERSION="0.0.6"
AC_SUBST(VERSION)

dnl Library version.
dnl current:revision:age
dnl  * On a new release, increment 'revision'.
dnl  * If you added things to the api, increment 'current',
dnl    reset 'revision' and increment 'age'.
dnl  * If you changed/removed things from the api, increment 'current',
dnl    reset 'revision' and reset 'age'.
LT_CURRENT=0
LT_REVISION=6
LT_AGE=0
LT_LIBVERSION="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(LT_LIBVERSION, $LT_CURRENT:$LT_REVISION:$LT_AGE)

AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr/local)

dnl Files to configure.
AC_CONFIG_SRCDIR([src/ticalcs.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  build/Makefile
  docs/Makefile
  intl/Makefile
  po/Makefile.in
  src/Makefile
  tests/Makefile
  ticalcs2.pc
])

# Setup libtool.
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_AWK
AC_CHECK_PROG(TFDOCGEN, tfdocgen, yes, no)
AM_CONDITIONAL(USE_DOCGEN, test "$TFDOCGEN" != "no")

# Checks for libraries.
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.14.5)

PKG_CHECK_MODULES(TICABLES, ticables2 >= 0.0.1)
AC_SUBST(TICABLES_CFLAGS)
AC_SUBST(TICABLES_LIBS)

PKG_CHECK_MODULES(TICONV, ticonv >= 0.0.1)
AC_SUBST(TICONV_CFLAGS)
AC_SUBST(TICONV_LIBS)

PKG_CHECK_MODULES(TIFILES, tifiles2 >= 0.0.9)
AC_SUBST(TIFILES_CFLAGS)
AC_SUBST(TIFILES_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h time.h unistd.h])
AC_CHECK_HEADERS([tilp2/ticables.h tilp2/tifiles.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS([bzero memmove memset strcasecmp strdup])

# Platform specific tests.
dnl AC_CANONICAL_HOST
case "$host" in
  *-*-*bsd*)  ARCH="-D__BSD__" ;;
  *-*-mingw*) ARCH="-D__WIN32__ -D__MINGW32__" ;;
  *)          ARCH="-D__LINUX__" ;;
esac
CFLAGS="$CFLAGS $ARCH"

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
	      AC_MSG_RESULT([${visibility_flag}])
	      AC_DEFINE(HAVE_FVISIBILITY, 1, [Use -fvisibility=hidden flag])
	    fi
	fi
	;;
esac

# Output.
AC_OUTPUT

echo "Now, you can type 'make' and 'make install'."
