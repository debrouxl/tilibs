## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticalcs.la
libticalcsincludedir = $(includedir)/tilp
libticalcsinclude_HEADERS = \
	calc_def.h calc_err.h calc_int.h calc_ver.h \
	headers.h \
	keys73.h keys83p.h keys89.h keys92p.h \
	pause.h \
	ticalcs.h \
	tikeys.h \
	tnode.h \
	update.h

# build instructions
libticalcs_la_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@
libticalcs_la_LIBADD = @LTLIBINTL@ @TICABLES_LIBS@ @TIFILES_LIBS@
libticalcs_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticalcs_la_SOURCES = *.h \
	clock.c \
	cmd73.c cmd82.c cmd85.c cmd89.c cmd92.c \
	dirlist.c \
	error.c \
	intrface.c \
	keys73.c keys83p.c keys89.c keys92p.c \
	packets.c \
	printl.c \
	probe.c \
	ti73.c ti82.c ti83.c ti85.c ti86.c ti89.c ti92.c \
	tikeys.c \
	tnode.c \
	type2str.c

dos2unix:
	dos2unix
	lowercase
	uppercase
