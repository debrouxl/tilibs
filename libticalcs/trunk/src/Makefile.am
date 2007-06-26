## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs2.la
libticalcsincludedir = $(includedir)/tilp2
libticalcsinclude_HEADERS = \
	ticalcs.h export3.h stdints3.h \
	keys73.h keys83p.h keys89.h keys92p.h

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
	clock.c \
	cmd73.c cmd82.c cmd85.c cmd89.c cmd92.c dusb_cmd.c \
	dirlist.c \
	error.c \
	keys73.c keys83.c keys83p.c keys86.c keys89.c keys92p.c \
	logging.c \
	dbus_pkt.c dusb_rpkt.c dusb_vpkt.c \
	probe.c \
	romdump.c \
	ticalcs.c \
	tikeys.c \
	type2str.c \
	update.c
