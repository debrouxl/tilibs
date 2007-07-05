## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libtifiles2.la
libtifilesincludedir = $(includedir)/tilp2
libtifilesinclude_HEADERS = \
	tifiles.h export2.h stdints2.h \
	files8x.h files9x.h types*.h 

# build instructions
libtifiles2_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ @TICONV_CFLAGS@ -DTIFILES_EXPORTS
libtifiles2_la_LIBADD = @GLIB_LIBS@ @LTLIBINTL@ @LIBZ@ @TICONV_LIBS@
libtifiles2_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libtifiles2_la_SOURCES = *.h minizip/*.h \
	comments.c \
	error.c \
	files8x.c files9x.c filesxx.c \
	filetypes.c \
	grouped.c \
	intelhex.c \
	logging.c \
	misc.c \
	rwfile.c \
	tifiles.c \
	tigroup.c \
	type2str.c \
	types73.c types82.c types83.c types83p.c types84p.c \
	types85.c types86.c \
	types89.c types89t.c types92.c types92p.c typesv2.c \
	typesxx.c \
	ve_fp.c \
	minizip/ioapi.c   \
        minizip/miniunz.c \
        minizip/minizip.c \
        minizip/unzip.c   \
        minizip/zip.c

if OS_WIN32
  libtifiles2_la_DEPENDENCIES = ../build/mingw/tifiles2-rc.o
  libtifiles2_la_LDFLAGS += -Wl,../build/mingw/tifiles2-rc.o
endif

EXTRA_DIST = minizip/*