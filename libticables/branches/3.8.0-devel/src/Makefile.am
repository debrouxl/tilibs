## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

DEFS=@DEFS@ -DLOCALEDIR=\"$(locale_dir)\"

INCLUDES = -I$(top_srcdir) -I$(includedir) \
        -I../intl -I$(top_srcdir)/intl \
        -DPREFIX=\"${prefix}\" -I.

# libraries to compile and install
lib_LTLIBRARIES = libticables.la

# libtool stuff: set version and export symbols for resolving
libticablesincludedir = $(includedir)/tilp
libticables_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic $(no_undefined)

# ticables header files for public installation (non-generated)
libticablesinclude_HEADERS = \
	cabl_def.h \
	cabl_err.h \
	cabl_int.h \
	cabl_ver.h \
	export.h \
	intl.h \
	timeout.h \
	ticables.h

# setup source file variables
libticables_la_SOURCES = \
	detect.c \
	direct.c \
	error.c \
	ioports.c \
	intrface.c \
	link_avr.c \
	link_dev.c \
	link_dfl.c \
	link_par.c \
	link_ser.c \
	link_slv.c \
	link_tie.c \
	link_tig.c \
	link_vti.c \
	link_vtl.c \
	logging.c \
	mapping.c \
	none.c \
	type2str.c \
	verbose.c

MAINTAINERCLEANFILES =
EXTRA_HEADERS =
EXTRA_DIST = \
	*.h \
	bsd linux macos win32

dos2unix:
	dos2unix
	lowercase
	uppercase