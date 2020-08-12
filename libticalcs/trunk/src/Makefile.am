## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -W

SUBDIRS = romdump_8x romdump_84p_usb romdump_834pce_usb romdump_9x

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs2.la
libticalcsincludedir = $(includedir)/tilp2
libticalcsinclude_HEADERS = \
	ticalcs.h export3.h \
	keys73.h keys83.h keys83p.h keys86.h keys89.h keys92p.h \
	dbus_pkt.h dusb_rpkt.h dusb_vpkt.h dusb_cmd.h nsp_rpkt.h nsp_vpkt.h nsp_cmd.h \
	cmdz80.h cmd68k.h romdump.h

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
endif


BUILT_SOURCES =

if BUILD_Z80_ROM_DUMPERS

BUILT_SOURCES += rom73.h rom82.h rom83.h rom83p.h rom84p.h rom84pc.h rom84pcu.h rom834pceu.h rom85.h rom86.h

rom73.h: romdump_8x/dump73.73p
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump73.73p -o $(srcdir)/rom73.h -n romDump73 -s romDumpSize73 -c 8 -m 64

rom82.h: romdump_8x/dump82.82y
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump82.82y -o $(srcdir)/rom82.h -n romDump82 -s romDumpSize82 -c 8 -m 64

rom83.h: romdump_8x/dump83.83p
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump83.83p -o $(srcdir)/rom83.h -n romDump83 -s romDumpSize83 -c 8 -m 64

rom83p.h: romdump_8x/dump83p.8xp
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump83p.8xp -o $(srcdir)/rom83p.h -n romDump8Xp -s romDumpSize8Xp -c 8 -m 64

rom84pc.h: romdump_8x/dump84pc.8xp
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump84pc.8xp -o $(srcdir)/rom84pc.h -n romDump84pc -s romDumpSize84pc -c 8 -m 64

rom85.h: romdump_8x/dump85.85s
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump85.85s -o $(srcdir)/rom85.h -n romDump85 -s romDumpSize85 -c 8 -m 64

rom86.h: romdump_8x/dump86.86p
	$(TIFILEUTIL) dump -f chexarray romdump_8x/dump86.86p -o $(srcdir)/rom86.h -n romDump86 -s romDumpSize86 -c 8 -m 64

romdump_8x/dump73.73p:
	cd romdump_8x && $(MAKE) dump73.73p

romdump_8x/dump82.82y:
	cd romdump_8x && $(MAKE) dump82.82y

romdump_8x/dump83.83p:
	cd romdump_8x && $(MAKE) dump83.83p

romdump_8x/dump83p.8xp:
	cd romdump_8x && $(MAKE) dump83p.8xp

romdump_8x/dump85.85s:
	cd romdump_8x && $(MAKE) dump85.85s

romdump_8x/dump86.86p:
	cd romdump_8x && $(MAKE) dump86.86p

romdump_8x/dump84pc.8xp:
	cd romdump_8x && $(MAKE) dump84pc.8xp

rom84p.h: romdump_84p_usb/dump84p.8xp
	$(TIFILEUTIL) dump -f chexarray romdump_84p_usb/dump84p.8xp -o $(srcdir)/rom84p.h -n romDump84p -s romDumpSize84p -c 8 -m 64

rom84pcu.h: romdump_84p_usb/dump84pcu.8xp
	$(TIFILEUTIL) dump -f chexarray romdump_84p_usb/dump84pcu.8xp -o $(srcdir)/rom84pcu.h -n romDump84pcu -s romDumpSize84pcu -c 8 -m 64

rom82a.h: romdump_84p_usb/dump82a.8xp $(tf2hex)
	$(TIFILEUTIL) dump -f chexarray romdump_84p_usb/dump82a.8xp -o $(srcdir)/rom82a.h -n romDump82a -s romDumpSize82a -c 8 -m 64

romdump_84p_usb/dump84p.8xp:
	cd romdump_84p_usb && $(MAKE) dump84p.8xp

romdump_84p_usb/dump84pcu.8xp:
	cd romdump_84p_usb && $(MAKE) dump84pcu.8xp

romdump_84p_usb/dump82a.8xp:
	cd romdump_84p_usb && $(MAKE) dump82a.8xp

rom834pceu.h: romdump_834pce_usb/dump834pceu.8xp
	$(TIFILEUTIL) dump -f chexarray romdump_834pce_usb/dump834pceu.8xp -o $(srcdir)/rom834pceu.h -n romDump834pceu -s romDumpSize834pceu -c 8 -m 64

romdump_834pce_usb/dump834pceu.8xp:
	cd romdump_834pce_usb && $(MAKE) dump834pceu.8xp

endif

if BUILD_68K_ROM_DUMPERS

BUILT_SOURCES += rom89.h rom89t.h rom92f2.h

rom89.h: romdump_9x/romdump.89z
	$(TIFILEUTIL) dump -f chexarray romdump_9x/romdump.89z -o $(srcdir)/rom89.h -n romDump89 -s romDumpSize89 -c 8 -m 64

rom89t.h: romdump_9x/romdumpu.89z
	$(TIFILEUTIL) dump -f chexarray romdump_9x/romdumpu.89z -o $(srcdir)/rom89t.h -n romDump89t -s romDumpSize89t -c 8 -m 64

rom92f2.h: romdump_9x/romdump.92p
	$(TIFILEUTIL) dump -f chexarray romdump_9x/romdump.92p -o $(srcdir)/rom92f2.h -n romDump92 -s romDumpSize92 -c 8 -m 64

romdump_9x/romdump.89z:
	cd romdump_9x && $(MAKE) romdump.89z

romdump_9x/romdump.92p:
	cd romdump_9x && $(MAKE) romdump.92p

romdump_9x/romdumpu.89z:
	cd romdump_9x && $(MAKE) romdumpu.89z

endif
