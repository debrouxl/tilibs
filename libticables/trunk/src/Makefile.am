## Process this file with automake to produce Makefile.in

DEFS=@DEFS@ -DLOCALEDIR=\"$(datadir)/locale\" -DPREFIX=\"$(prefix)\"
AM_CPPFLAGS = -I$(top_srcdir)/intl @LIBUSB_CFLAGS@
AM_CFLAGS = -Wall

# libraries to compile and install
lib_LTLIBRARIES = libticables.la

# build instructions
libticables_la_LIBADD = @LTLIBINTL@ @LIBUSB_LIBS@
libticables_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables_la_SOURCES = \
	detect.c detect.h \
	direct.c \
	error.c \
	externs.h \
	gettext.h \
	intrface.c \
	ioports.c ioports.h \
	link_avr.c \
	link_dev.c \
	link_dfl.c \
	link_par.c \
	link_ser.c \
	link_slv.c \
	link_tie.c \
	link_tig.c \
	link_vti.c \
	link_vtl.c \
	link_xxx.h \
	logging.c logging.h \
	mapping.c mapping.h \
	none.c none.h \
	printl.c printl.h \
	stdints.h \
	type2str.c type2str.h \
	verbose.c verbose.h

# ticables header files for public installation (non-generated)
libticablesincludedir = $(includedir)/tilp
libticablesinclude_HEADERS = \
	cabl_def.h \
	cabl_err.h \
	cabl_int.h \
	cabl_ver.h \
	export.h \
	ticables.h \
	timeout.h

EXTRA_DIST = bsd/* linux/* macos/* win32/*

dos2unix:
	dos2unix
	lowercase
	uppercase
