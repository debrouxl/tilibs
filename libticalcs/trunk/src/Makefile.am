## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

DEFS=@DEFS@ -DLOCALEDIR=\"$(locale_dir)\"

INCLUDES = -I$(top_srcdir) -I$(includedir) \
        -I../intl -I$(top_srcdir)/intl \
        -DPREFIX=\"${prefix}\" -I.

# libraries to compile and install
lib_LTLIBRARIES = libticalcs-4.la

# libtool stuff: set version and export symbols for resolving
libticalcsincludedir = $(includedir)/tilp
libticalcs_4_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic $(no_undefined)

# ticalcs header files for public installation (non-generated)
libticalcsinclude_HEADERS = \
	calc_def.h \
	calc_err.h \
	calc_int.h \
	calc_ver.h \
	headers.h \
	intl2.h \
	keys73.h \
	keys83p.h \
	keys89.h \
	keys92p.h \
	pause.h \
	update.h \
	ticalcs.h \
	tikeys.h

# setup source file variables
libticalcs_4_la_SOURCES = \
	clock.c \
	cmd73.c \
	cmd82.c \
	cmd85.c \
	cmd89.c \
	cmd92.c \
	dirlist.c \
	error.c \
	intrface.c \
	keys73.c \
	keys83p.c \
	keys89.c \
	keys92p.c \
	packets.c \
	probe.c \
	ti73.c \
	ti82.c \
	ti83.c \
	ti85.c \
	ti86.c \
	ti89.c \
	ti92.c \
	tikeys.c \
	type2str.c

# objects
libticalcs_4_OBJECTS = \
        error.o

MAINTAINERCLEANFILES =
EXTRA_HEADERS =
EXTRA_DIST = *.h

dos2unix:
	dos2unix
	lowercase
	uppercase