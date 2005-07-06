## Process this file with automake to produce Makefile.in

DEFS = @DEFS@ -DLOCALEDIR=\"$(datadir)/locale\"
AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs2.la
libticalcsincludedir = $(includedir)/tilp2
libticalcsinclude_HEADERS = \
	ticalcs.h export.h \
	keys73.h keys83p.h keys89.h keys92p.h \
	tnode.h

# build instructions
libticalcs2_la_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@
libticalcs2_la_LIBADD = @LTLIBINTL@ @TICABLES_LIBS@ @TIFILES_LIBS@
libticalcs2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticalcs2_la_SOURCES = *.h \
	calc_00.c calc_xx.c \
	calc_73.c calc_82.c calc_83.c calc_85.c calc_86.c \
	calc_89.c calc_92.c \
	clock.c \
	cmd73.c cmd82.c cmd85.c cmd89.c cmd92.c \
	dirlist.c \
	error.c \
	keys73.c keys83p.c keys89.c keys92p.c \
	logging.c \
	packets.c \
	probe.c \
	ticalcs.c \
	tikeys.c \
	tnode.c \
	type2str.c \
	update.c
