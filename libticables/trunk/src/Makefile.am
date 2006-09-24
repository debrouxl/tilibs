## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables2.la
libticablesincludedir = $(includedir)/tilp2
libticablesinclude_HEADERS = ticables.h export1.h stdints1.h timeout.h

# build instructions
libticables2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @LIBUSB_CFLAGS@ -DTICABLES_EXPORTS
libticables2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBUSB_LIBS@
libticables2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@x
libticables2_la_SOURCES = *.h \
	data_log.c \
	detect.c \
	error.c \
	ioports.c \
	link_gry.c link_nul.c link_par.c link_blk.c \
	link_slv.c link_tie.c link_vti.c link_xxx.c \
	log_dbus.c log_dusb.c log_hex.c log2pkt.c \
	logging.c \
	probe.c \
	ticables.c \
	type2str.c

EXTRA_DIST = bsd/* linux/* macos/* win32/*

wc:
	wc *.c *.h ./linux/*.c ./linux/*.h ./win32/*.c ./win32/*.h ./bsd/*.c ./bsd/*.h ./macos/*.c ./macos/*.h
