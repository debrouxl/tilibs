## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables.la
libticablesincludedir = $(includedir)/tilp
libticablesinclude_HEADERS = \
	cabl_def.h cabl_err.h cabl_int.h cabl_ver.h \
	export.h \
	ticables.h \
	timeout.h

# build instructions
libticables_la_CPPFLAGS = -DLOCALEDIR=\"$(DESTDIR)$(datadir)/locale\" \
	@LIBUSB_CFLAGS@
libticables_la_LIBADD = @LTLIBINTL@ @LIBUSB_LIBS@
libticables_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables_la_SOURCES = *.h \
	detect.c \
	direct.c \
	error.c \
	intrface.c \
	ioports.c \
	link_avr.c link_dev.c link_nul.c link_par.c link_ser.c \
	link_slv.c link_tie.c link_tig.c link_vti.c link_vtl.c \
	logging.c \
	mapping.c \
	none.c \
	printl.c \
	type2str.c \
	verbose.c

EXTRA_DIST = bsd/* linux/* macos/* win32/*

dos2unix:
	dos2unix
	lowercase
	uppercase
