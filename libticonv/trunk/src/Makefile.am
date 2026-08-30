## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticonv.la
libticonvincludedir = $(includedir)/tilp2
libticonvinclude_HEADERS = \
	ticonv.h export4.h charset.h

# build instructions
libticonv_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ -DTICONV_EXPORTS
libticonv_la_LIBADD = @GLIB_LIBS@
libticonv_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticonv_la_SOURCES = \
	ticonv.cc charset.cc tokens.cc filename.cc type2str.cc iconv.c

if OS_WIN32
if USE_ICONV
  libticonv_la_LIBADD += -liconv
endif
  libticonv_la_DEPENDENCIES = ../build/mingw/ticonv-rc.o
  libticonv_la_LDFLAGS += -Wl,../build/mingw/ticonv-rc.o -Wl,--output-def,libticonv.def
  EXTRA_DIST = libticonv.def
endif

if USE_ICONV
if OS_MAC
  libticonv_la_CPPFLAGS += -I/usr/local/opt/libiconv/include/
  libticonv_la_LDFLAGS += -L/usr/local/opt/libiconv/lib/
  libticonv_la_LIBADD += -liconv
endif
  libticonv_la_CPPFLAGS += -DUSE_ICONV
endif