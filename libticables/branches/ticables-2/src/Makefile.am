## Process this file with automake to produce Makefile.in

DEFS = @DEFS@ -DLOCALEDIR=\"$(datadir)/locale\"
AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables2.la
libticablesincludedir = $(includedir)/tilp2
libticablesinclude_HEADERS = ticables.h export.h stdints.h timeout.h

# build instructions
libticables2_la_CPPFLAGS = @GLIB_CFLAGS@ @LIBUSB_CFLAGS@ \
	-DLOCALEDIR=\"$(datadir)/locale\"
libticables2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBUSB_LIBS@
libticables2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables2_la_SOURCES = *.h \
	data_log.c \
	detect.c \
	error.c \
	ioports.c \
	link_gry.c link_nul.c link_par.c link_blk.c \
	link_slv.c link_tie.c link_vti.c link_xxx.c \
	logging.c \
	ticables.c \
	type2str.c

EXTRA_DIST = bsd/* linux/* macos/* win32/*
