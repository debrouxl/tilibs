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
	ioports.c \
	avr_link.c \
	dev_link.c \
	dfl_link.c \
	par_link.c \
	ser_link.c \
	slv_link.c \
	tie_link.c \
	tig_link.c \
	vti_link.c \
	vtl_link.c \
	error.c intrface.c type2str.c \
	logging.c probe.c verbose.c \
	direct.c \
	timodules.c

MAINTAINERCLEANFILES =
EXTRA_HEADERS =
EXTRA_DIST = *.h

dos2unix:
	dos2unix
	lowercase
	uppercase