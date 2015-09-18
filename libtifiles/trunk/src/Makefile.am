## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libtifiles2.la
libtifilesincludedir = $(includedir)/tilp2
libtifilesinclude_HEADERS = \
	tifiles.h export2.h stdints2.h \
	files8x.h files9x.h types73.h \
	types82.h types83.h types83p.h \
	types84p.h types85.h types86.h \
	types89.h types89t.h types92.h \
	types92p.h typesnsp.h typesv2.h \
	typesxx.h

# build instructions
libtifiles2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICONV_CFLAGS@ @ARCHIVE_CFLAGS@ -DTIFILES_EXPORTS
libtifiles2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @ARCHIVE_LIBS@ @TICONV_LIBS@
libtifiles2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libtifiles2_la_SOURCES = *.h \
	comments.c \
	error.c \
	files8x.c files9x.c filesnsp.c filesxx.c \
	filetypes.c \
	grouped.c \
	intelhex.c \
	logging.c \
	misc.c \
	rwfile.c \
	tifiles.c \
	tigroup.c \
	type2str.c \
	typesoldz80.c \
	types83p.c \
	types68k.c \
	typesnsp.c \
	typesxx.c \
	ve_fp.c

if OS_WIN32
  libtifiles2_la_DEPENDENCIES = ../build/mingw/tifiles2-rc.o
  libtifiles2_la_LDFLAGS += -Wl,../build/mingw/tifiles2-rc.o -Wl,--output-def,libtifiles2.def
  EXTRA_DIST = libtifiles2.def
endif
