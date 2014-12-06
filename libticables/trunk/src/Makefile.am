## Process this file with automake to produce Makefile.in

SUBDIRS = win32/dha win64/rwp

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables2.la
libticablesincludedir = $(includedir)/tilp2
libticablesinclude_HEADERS = ticables.h export1.h stdints1.h timeout.h

# build instructions
libticables2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @LIBUSB_CFLAGS@ @LIBUSB10_CFLAGS@ -DTICABLES_EXPORTS
libticables2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBUSB_LIBS@ @LIBUSB10_LIBS@
libticables2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables2_la_SOURCES = *.h \
	data_log.c \
	detect.c \
	error.c \
	ioports.c \
	link_gry.c link_nul.c link_par.c link_blk.c \
	link_usb.c link_tie.c link_vti.c link_xxx.c \
	log_dbus.c log_dusb.c log_hex.c log_nsp.c \
	hex2dbus.c hex2dusb.c hex2nsp.c \
	logging.c \
	probe.c \
	ticables.c \
	type2str.c \
	none.c

EXTRA_DIST = bsd/* linux/* macos/* win32/*.c win32/*.h win32/dha/* win32/usb/* win64/*.c win64/*.h win64/rwp/*

if OS_WIN32
  libticables2_la_SOURCES += win32/libusb_dyn.c

  libticables2_la_DEPENDENCIES = ../build/mingw/ticables2-rc.o
  libticables2_la_LDFLAGS += -Wl,../build/mingw/ticables2-rc.o -Wl,--output-def,libticables2.def
  EXTRA_DIST += libticables2.def

  libusbdir = $(pkgdatadir)/libusb
  dist_libusb_DATA = win32/usb/*.cat win32/usb/*.inf
else
  EXTRA_DIST += win32/usb/*.cat win32/usb/*.inf
endif

wc:
	wc *.c *.h ./linux/*.c ./linux/*.h ./win32/*.c ./win32/*.h ./win64/*.c ./win64/*.h ./bsd/*.c ./bsd/*.h ./macos/*.c ./macos/*.h
