## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

DEFS=@DEFS@ -DLOCALEDIR=\"$(locale_dir)\"

INCLUDES = -I$(top_srcdir) -I$(includedir) \
        -I../intl -I$(top_srcdir)/intl \
        -DPREFIX=\"${prefix}\" -I.

# libraries to compile and install
lib_LTLIBRARIES = libtifiles.la

# libtool stuff: set version and export symbols for resolving
libtifilesincludedir = $(includedir)/tilp
libtifiles_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic $(no_undefined)

# tifiles header files for public installation (non-generated)
libtifilesinclude_HEADERS = \
	file_def.h \
	file_err.h \
	file_int.h \
	file_ver.h \
	files8x.h \
	files9x.h \
	filesxx.h \
	grp_ops.h \
	macros.h \
	stdints.h \
	typesxx.h \
	types73.h \
	types82.h \
	types83.h \
	types83p.h \
	types85.h \
	types86.h \
	types89.h \
	types92.h \
	types92p.h \
	typesv2.h \
	tifiles.h

# setup source file variables
libtifiles_la_SOURCES = \
	error.c \
	files8x.c \
	files9x.c \
	filesxx.c \
	grp_ops.c \
	intelhex.c \
	intrface.c \
	misc.c \
	trans.c \
	types73.c \
	types82.c \
	types83.c \
	types83p.c \
	types85.c \
	types86.c \
	types89.c \
	types92.c \
	types92p.c \
	typesv2.c \
	typesxx.c \
	type2str.c

MAINTAINERCLEANFILES =
EXTRA_HEADERS =
EXTRA_DIST = *.h

dos2unix:
	dos2unix
	lowercase
	uppercase