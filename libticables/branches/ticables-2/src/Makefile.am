## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables-2.la
libticablesincludedir = $(includedir)/tilp2
libticablesinclude_HEADERS = ticables.h exports.h stdints.h timeout.h

# build instructions
libticables_2_la_CPPFLAGS = @GLIB_CFLAGS@ @LIBUSB_CFLAGS@
libticables_2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBUSB_LIBS@
libticables_2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables_2_la_SOURCES = *.h \
	data_log.c \
	detect.c \
	error.c \
	ioports.c \
	link_gry.c link_nul.c link_par.c link_ser.c \
	link_slv.c link_tie.c link_vti.c link_xxx.c \
	logging.c \
	ticables.c \
	type2str.c

EXTRA_DIST = bsd/* linux/* macos/* win32/*

dos2unix:
	dos2unix
	lowercase
	uppercase
