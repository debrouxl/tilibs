## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

DEFS=@DEFS@ -DLOCALEDIR=\"$(locale_dir)\"

INCLUDES = -I$(top_srcdir) -I$(includedir) \
        -I../intl -I$(top_srcdir)/intl \
        -DPREFIX=\"${prefix}\" -I.

# libraries to compile and install
lib_LTLIBRARIES = libticalcs.la

# libtool stuff: set version and export symbols for resolving
libticalcsincludedir = $(includedir)/tilp
libticalcs_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic $(no_undefined)

# ticalcs header files for public installation (non-generated)
libticalcsinclude_HEADERS = \
	../src/calc_def.h \
	../src/calc_err.h \
	../src/calc_int.h \
	../src/calc_ver.h \
	../src/headers.h \
	../src/intl2.h \
	../src/keys73.h \
	../src/keys83p.h \
	../src/keys89.h \
	../src/keys92p.h \
	../src/pause.h \
	../src/update.h \
	../src/ticalcs.h \
	../src/tikeys.h

# setup source file variables
libticalcs_la_SOURCES = \
	../src/clock.c \
	../src/cmd73.c \
	../src/cmd82.c \
	../src/cmd85.c \
	../src/cmd89.c \
	../src/cmd92.c \
	../src/dirlist.c \
	../src/error.c \
	../src/intrface.c \
	../src/keys73.c \
	../src/keys83p.c \
	../src/keys89.c \
	../src/keys92p.c \
	../src/packets.c \
	../src/probe.c \
	../src/ti73.c \
	../src/ti82.c \
	../src/ti83.c \
	../src/ti85.c \
	../src/ti86.c \
	../src/ti89.c \
	../src/ti92.c \
	../src/tikeys.c \
	../src/type2str.c

# objects
libticalcs_OBJECTS = \
        error.o

MAINTAINERCLEANFILES =
EXTRA_HEADERS =
EXTRA_DIST = ../src/*.h

dos2unix:
	dos2unix
	lowercase
	uppercase