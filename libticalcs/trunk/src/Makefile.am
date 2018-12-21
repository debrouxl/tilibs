## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -W

SUBDIRS = romdump_8x romdump_84p_usb romdump_834pce_usb romdump_9x tf2hex

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs2.la
libticalcsincludedir = $(includedir)/tilp2
libticalcsinclude_HEADERS = \
	ticalcs.h export3.h \
	keys73.h keys83.h keys83p.h keys86.h keys89.h keys92p.h \
	dbus_pkt.h dusb_rpkt.h dusb_vpkt.h dusb_cmd.h nsp_rpkt.h nsp_vpkt.h nsp_cmd.h \
	cmdz80.h cmd68k.h

# build instructions
libticalcs2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICABLES_CFLAGS@ @TIFILES_CFLAGS@ @TICONV_CFLAGS@ -DTICALCS_EXPORTS
libticalcs2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@
libticalcs2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticalcs2_la_SOURCES = *.h \
	backup.cc \
	calc_00.cc calc_xx.cc \
	calc_73.cc calc_8x.cc calc_9x.cc \
	calc_84p.cc calc_89t.cc \
	calc_nsp.cc \
	clock.cc \
	cmdz80.cc cmd68k.cc dusb_cmd.cc nsp_cmd.cc \
	dirlist.cc \
	error.cc \
	keys73.cc keys83.cc keys83p.cc keys86.cc keys89.cc keys92p.cc \
	dbus_pkt.cc \
	dusb_rpkt.cc dusb_vpkt.cc \
	nsp_rpkt.cc nsp_vpkt.cc \
	probe.cc \
	romdump.cc \
	screen.cc \
	ticalcs.cc \
	tikeys.cc \
	type2str.cc \
	update.cc

if OS_WIN32
  libticalcs2_la_DEPENDENCIES = ../build/mingw/ticalcs2-rc.o
  libticalcs2_la_LDFLAGS += -Wl,../build/mingw/ticalcs2-rc.o -Wl,--output-def,libticalcs2.def
  EXTRA_DIST = libticalcs2.def
  tf2hex = ./tf2hex/tf2hex.exe
else
  tf2hex = ./tf2hex/tf2hex
endif


BUILT_SOURCES =

if BUILD_Z80_ROM_DUMPERS

BUILT_SOURCES += rom73.h rom82.h rom83.h rom83p.h rom84p.h rom84pc.h rom84pcu.h rom834pceu.h rom85.h rom86.h

rom73.h: romdump_8x/dump73.73p $(tf2hex)
	$(tf2hex) romdump_8x/dump73.73p $(srcdir)/rom73.h 73

rom82.h: romdump_8x/dump82.82y $(tf2hex)
	$(tf2hex) romdump_8x/dump82.82y $(srcdir)/rom82.h 82

rom83.h: romdump_8x/dump83.83p $(tf2hex)
	$(tf2hex) romdump_8x/dump83.83p $(srcdir)/rom83.h 83

rom83p.h: romdump_8x/dump83p.8xp $(tf2hex)
	$(tf2hex) romdump_8x/dump83p.8xp $(srcdir)/rom83p.h 8Xp

rom84pc.h: romdump_8x/dump84pc.8xp $(tf2hex)
	$(tf2hex) romdump_8x/dump84pc.8xp $(srcdir)/rom84pc.h 84pc

rom85.h: romdump_8x/dump85.85s $(tf2hex)
	$(tf2hex) romdump_8x/dump85.85s $(srcdir)/rom85.h 85

rom86.h: romdump_8x/dump86.86p $(tf2hex)
	$(tf2hex) romdump_8x/dump86.86p $(srcdir)/rom86.h 86

romdump_8x/dump73.73p romdump_8x/dump82.82y romdump_8x/dump83.83p romdump_8x/dump83p.8xp romdump_8x/dump85.85s romdump_8x/dump86.86p romdump_8x/dump84pc.8xp:
	cd romdump_8x && $(MAKE)

rom84p.h: romdump_84p_usb/dump84p.8xp $(tf2hex)
	$(tf2hex) romdump_84p_usb/dump84p.8xp $(srcdir)/rom84p.h 84p

rom84pcu.h: romdump_84p_usb/dump84pcu.8xp $(tf2hex)
	$(tf2hex) romdump_84p_usb/dump84pcu.8xp $(srcdir)/rom84pcu.h 84pcu

romdump_84p_usb/dump84p.8xp romdump_84p_usb/dump84pcu.8xp:
	cd romdump_84p_usb && $(MAKE)

rom834pceu.h: romdump_834pce_usb/dump834pceu.8xp $(tf2hex)
	$(tf2hex) romdump_834pce_usb/dump834pceu.8xp $(srcdir)/rom834pceu.h 834pceu

romdump_834pce_usb/dump834pceu.8xp:
	cd romdump_834pce_usb && $(MAKE)

endif

if BUILD_68K_ROM_DUMPERS

BUILT_SOURCES += rom89.h rom89t.h rom92f2.h

rom89.h: romdump_9x/romdump.89z $(tf2hex)
	$(tf2hex) romdump_9x/romdump.89z $(srcdir)/rom89.h 89

rom89t.h: romdump_9x/romdumpu.89z $(tf2hex)
	$(tf2hex) romdump_9x/romdumpu.89z $(srcdir)/rom89t.h 89t

rom92f2.h: romdump_9x/romdump.92p $(tf2hex)
	$(tf2hex) romdump_9x/romdump.92p $(srcdir)/rom92f2.h 92

romdump_9x/romdump.89z:
	cd romdump_9x && $(MAKE)

romdump_9x/romdump.92p: romdump_9x/romdump.89z
	cd romdump_9x && $(MAKE)

romdump_9x/romdumpu.89z:
	cd romdump_9x && $(MAKE)

endif
