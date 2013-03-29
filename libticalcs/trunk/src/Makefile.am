## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -W

SUBDIRS = tf2hex romdump_8x romdump_84p_usb romdump_9x romdump_89t_usb

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs2.la
libticalcsincludedir = $(includedir)/tilp2
libticalcsinclude_HEADERS = \
	ticalcs.h export3.h stdints3.h \
	keys73.h keys83.h keys83p.h keys86.h keys89.h keys92p.h \
	dbus_pkt.h dusb_rpkt.h dusb_vpkt.h dusb_cmd.h nsp_vpkt.h nsp_cmd.h \
	cmd73.h

# build instructions
libticalcs2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICABLES_CFLAGS@ @TIFILES_CFLAGS@ @TICONV_CFLAGS@ -DTICALCS_EXPORTS
libticalcs2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@
libticalcs2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticalcs2_la_SOURCES = *.h \
	backup.c \
	calc_00.c calc_xx.c \
	calc_73.c calc_82.c calc_83.c calc_85.c calc_86.c \
	calc_89.c calc_92.c \
	calc_84p.c calc_89t.c \
	calc_nsp.c \
	calc_80.c \
	clock.c \
	cmd73.c cmd80.c cmd82.c cmd85.c cmd89.c cmd92.c dusb_cmd.c nsp_cmd.c \
	dirlist.c \
	error.c \
	keys73.c keys83.c keys83p.c keys86.c keys89.c keys92p.c \
	logging.c \
	dbus_pkt.c \
	dusb_rpkt.c dusb_vpkt.c \
	nsp_rpkt.c nsp_vpkt.c \
	probe.c \
	romdump.c \
	ticalcs.c \
	tikeys.c \
	type2str.c \
	update.c

if OS_WIN32
  libticalcs2_la_DEPENDENCIES = ../build/mingw/ticalcs2-rc.o
  libticalcs2_la_LDFLAGS += -Wl,../build/mingw/ticalcs2-rc.o
endif

tf2hex = ./tf2hex/tf2hex

BUILT_SOURCES =

if BUILD_Z80_ROM_DUMPERS

BUILT_SOURCES += rom73.h rom82.h rom83.h rom83p.h rom84p.h rom85.h rom86.h

rom73.h: romdump_8x/dump73.73p $(tf2hex)
	$(tf2hex) romdump_8x/dump73.73p $(srcdir)/rom73.h 73

rom82.h: romdump_8x/dump82.82y $(tf2hex)
	$(tf2hex) romdump_8x/dump82.82y $(srcdir)/rom82.h 82

rom83.h: romdump_8x/dump83.83p $(tf2hex)
	$(tf2hex) romdump_8x/dump83.83p $(srcdir)/rom83.h 83

rom83p.h: romdump_8x/dump83p.8xp $(tf2hex)
	$(tf2hex) romdump_8x/dump83p.8xp $(srcdir)/rom83p.h 8Xp

rom85.h: romdump_8x/dump85.85s $(tf2hex)
	$(tf2hex) romdump_8x/dump85.85s $(srcdir)/rom85.h 85

rom86.h: romdump_8x/dump86.86p $(tf2hex)
	$(tf2hex) romdump_8x/dump86.86p $(srcdir)/rom86.h 86

romdump_8x/dump73.73p romdump_8x/dump82.82y romdump_8x/dump83.83p romdump_8x/dump83p.8xp romdump_8x/dump85.85s romdump_8x/dump86.86p:
	cd romdump_8x && $(MAKE)

rom84p.h: romdump_84p_usb/dump84p.8xp $(tf2hex)
	$(tf2hex) romdump_84p_usb/dump84p.8xp $(srcdir)/rom84p.h 84p

romdump_84p_usb/dump84p.8xp:
	cd romdump_84p_usb && $(MAKE)

endif

if BUILD_68K_ROM_DUMPERS

BUILT_SOURCES += rom89.h rom89t.h rom92f2.h

rom89.h: romdump_9x/romdump.89z $(tf2hex)
	$(tf2hex) romdump_9x/romdump.89z $(srcdir)/rom89.h 89

rom89t.h: romdump_89t_usb/romdump.89z $(tf2hex)
	$(tf2hex) romdump_89t_usb/romdump.89z $(srcdir)/rom89t.h 89t

rom92f2.h: romdump_9x/romdump.92p $(tf2hex)
	$(tf2hex) romdump_9x/romdump.92p $(srcdir)/rom92f2.h 92

romdump_9x/romdump.89z romdump_9x/romdump.92p:
	cd romdump_9x && $(MAKE)

romdump_89t_usb/romdump.89z:
	cd romdump_89t_usb && $(MAKE)

endif
