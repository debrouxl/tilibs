## Process this file with automake to produce Makefile.in

DEFS = @DEFS@ -DLOCALEDIR=\"$(datadir)/locale\" -DPREFIX=\"$(prefix)\"
AM_CPPFLAGS = -I$(top_srcdir)/intl
AM_CFLAGS = -Wall

# libraries to compile and install
lib_LTLIBRARIES = libtifiles.la

# build instructions
libtifiles_la_LIBADD = @LTLIBINTL@
libtifiles_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libtifiles_la_SOURCES = \
	error.c \
	export.h \
	files8x.c \
	files9x.c \
	filesxx.c \
	gettext.h \
	grp_ops.c \
	intelhex.c intelhex.h \
	intrface.c \
	misc.c misc.h \
	plerror.h \
	printl.c printl.h \
	trans.c trans.h \
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

# tifiles header files for public installation (non-generated)
libtifilesincludedir = $(includedir)/tilp
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
	tifiles.h \
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
	typesxx.h

dos2unix:
	dos2unix
	lowercase
	uppercase
