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
	ticonv.c charset.c tokens.c filename.c type2str.c iconv.c

if OS_WIN32
  libticonv_la_LIBADD += -liconv
  libticonv_la_DEPENDENCIES = ../build/mingw/ticonv-rc.o
  libticonv_la_LDFLAGS += -Wl,../build/mingw/ticonv-rc.o -Wl,--output-def,libticonv.def
  EXTRA_DIST = libticonv.def
endif

if OS_MAC
  libticonv_la_LIBADD += -liconv
endif
