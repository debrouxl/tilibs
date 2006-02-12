## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticonv.la
libticonvincludedir = $(includedir)/tilp2
libticonvinclude_HEADERS = \
	ticonv.h export4.h

# build instructions
libticon2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@
libticonv_la_LIBADD = @GLIB_LIBS@
libticonv_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticonv_la_SOURCES = \
	ticonv.c charset.c

EXTRA_DIST = 