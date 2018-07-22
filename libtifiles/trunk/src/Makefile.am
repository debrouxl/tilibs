## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libtifiles2.la
libtifilesincludedir = $(includedir)/tilp2
libtifilesinclude_HEADERS = \
	tifiles.h export2.h \
	files8x.h files9x.h filesnsp.h \
	types73.h types82.h types83.h types83p.h types84p.h types85.h types86.h \
	types89.h types89t.h types92.h types92p.h \
	typesnsp.h typesv2.h \
	typesxx.h

# build instructions
libtifiles2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICONV_CFLAGS@ @ARCHIVE_CFLAGS@ -DTIFILES_EXPORTS
libtifiles2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @ARCHIVE_LIBS@ @TICONV_LIBS@
libtifiles2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libtifiles2_la_SOURCES = *.h \
	cert.cc \
	comments.cc \
	error.cc \
	files8x.cc files9x.cc filesnsp.cc filesxx.cc \
	filetypes.cc \
	grouped.cc \
	intelhex.cc \
	misc.cc \
	rwfile.cc \
	tifiles.cc \
	tigroup.cc \
	type2str.cc \
	typesoldz80.cc \
	types83p.cc \
	types68k.cc \
	typesnsp.cc \
	typesxx.cc \
	ve_fp.cc

if OS_WIN32
  libtifiles2_la_DEPENDENCIES = ../build/mingw/tifiles2-rc.o
  libtifiles2_la_LDFLAGS += -Wl,../build/mingw/tifiles2-rc.o -Wl,--output-def,libtifiles2.def
  EXTRA_DIST = libtifiles2.def
endif
