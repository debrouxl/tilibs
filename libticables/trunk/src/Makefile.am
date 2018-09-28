## Process this file with automake to produce Makefile.in

SUBDIRS = win32/dha win64/rwp

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticables2.la
libticablesincludedir = $(includedir)/tilp2
libticablesinclude_HEADERS = ticables.h export1.h timeout.h

# build instructions
libticables2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @LIBUSB_CFLAGS@ @LIBUSB10_CFLAGS@ -DTICABLES_EXPORTS
libticables2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBUSB_LIBS@ @LIBUSB10_LIBS@
libticables2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticables2_la_SOURCES = *.h \
	data_log.cc \
	detect.cc \
	error.cc \
	ioports.cc \
	link_gry.cc link_nul.cc link_par.cc link_blk.cc \
	link_usb.cc link_tie.cc link_vti.cc link_xxx.cc \
	link_tcpc.cc link_tcps.cc \
	log_dbus.cc log_dusb.cc log_hex.cc log_nsp.cc \
	hex2dbus.cc hex2dusb.cc hex2nsp.cc \
	probe.cc \
	ticables.cc \
	type2str.cc \
	none.cc

EXTRA_DIST = bsd/* linux/* macos/* win32/*.cc win32/*.h win32/dha.c win32/dha/* win32/usb/* win64/*.c win64/*.cc win64/*.h win64/rwp.c win64/rwp/*

if OS_WIN32
  libticables2_la_SOURCES += win32/libusb_dyn.cc

  libticables2_la_DEPENDENCIES = ../build/mingw/ticables2-rc.o
  libticables2_la_LDFLAGS += -Wl,../build/mingw/ticables2-rc.o -Wl,--output-def,libticables2.def
  EXTRA_DIST += libticables2.def

  libusbdir = $(pkgdatadir)/libusb
  dist_libusb_DATA = win32/usb/*.cat win32/usb/*.inf
else
  EXTRA_DIST += win32/usb/*.cat win32/usb/*.inf
endif

wc:
	wc *.cc *.h ./linux/*.cc ./linux/*.h ./win32/*.cc ./win32/*.c ./win32/*.h ./win32/dha/*.c ./win32/dha/*.h ./win64/*.cc ./win64/*.c ./win64/*.h ./win64/rwp/*.c ./win64/rwp/*.h ./bsd/*.cc ./bsd/*.h ./macos/*.cc ./macos/*.h
