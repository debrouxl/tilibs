## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -W

SUBDIRS =

# libraries and files to compile and install
lib_LTLIBRARIES = libtiopers.la
libtiopersincludedir = $(includedir)/tilp2
libtiopersinclude_HEADERS = \
	tiopers.h export5.h

# build instructions
libtiopers_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICABLES_CFLAGS@ @TIFILES_CFLAGS@ @TICONV_CFLAGS@ -DTIOPERS_EXPORTS
libtiopers_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@
libtiopers_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libtiopers_la_SOURCES = *.h \
	device.c \
	error.c \
	opers_xx.c \
	tiopers.c

if OS_WIN32
  libtiopers_la_DEPENDENCIES = ../build/mingw/tiopers-rc.o
  libtiopers_la_LDFLAGS += -Wl,../build/mingw/tiopers-rc.o
endif
