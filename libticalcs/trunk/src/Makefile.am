## Process this file with automake to produce Makefile.in

DEFS = @DEFS@ -DLOCALEDIR=\"$(DESTDIR)$(datadir)/locale\"
AM_CPPFLAGS = -I$(top_srcdir)/intl @TICABLES_CFLAGS@ @TIFILES_CFLAGS@
AM_CFLAGS = -Wall

# libraries to compile and install
lib_LTLIBRARIES = libticalcs.la

# build instructions
libticalcs_la_LIBADD = @LTLIBINTL@ @TICABLES_LIBS@ @TIFILES_LIBS@
libticalcs_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticalcs_la_SOURCES = \
	clock.c clock.h \
	cmd73.c cmd73.h \
	cmd82.c cmd82.h \
	cmd85.c cmd85.h \
	cmd89.c cmd89.h \
	cmd92.c cmd92.h \
	dirlist.c dirlist.h \
	error.c \
	export.h \
	externs.h \
	gettext.h \
	intrface.c \
	keys73.c \
	keys83p.c \
	keys89.c \
	keys92p.c \
	packets.c packets.h \
	printl.c printl.h \
	probe.c \
	rom82.h \
	rom83.h \
	rom83p.h \
	rom85u.h \
	rom85z.h \
	rom86.h \
	rom89.h \
	rom92f2.h \
	ti73.c ti73.h \
	ti82.c ti82.h \
	ti83.c ti83.h \
	ti85.c ti85.h \
	ti86.c ti86.h \
	ti89.c ti89.h \
	ti92.c ti92.h \
	tikeys.c \
	tixx.h \
	tnode.c \
	type2str.c type2str.h

# ticalcs header files for public installation (non-generated)
libticalcsincludedir = $(includedir)/tilp
libticalcsinclude_HEADERS = \
	calc_def.h \
	calc_err.h \
	calc_int.h \
	calc_ver.h \
	headers.h \
	keys73.h \
	keys83p.h \
	keys89.h \
	keys92p.h \
	pause.h \
	ticalcs.h \
	tikeys.h \
	tnode.h \
	update.h

dos2unix:
	dos2unix
	lowercase
	uppercase
