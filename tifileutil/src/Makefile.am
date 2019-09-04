## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

# programs and files to compile and install
bin_PROGRAMS = tifileutil

# build instructions
tifileutil_CPPFLAGS = -I$(top_srcdir)/intl \
	@TIFILES_CFLAGS@ @TICONV_CFLAGS@ \
	@GLIB_CFLAGS@ \
	-DSHARE_DIR=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-DSYSCONFDIR=\"$(sysconfdir)\"
tifileutil_LDFLAGS =
tifileutil_LDADD = @GLIB_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@
tifileutil_SOURCES = *.h \
	main.cc

if OS_WIN32
  tifileutil_DEPENDENCIES = ../build/mingw/tifileutil-rc.o
  tifileutil_LDFLAGS += -Wl,../build/mingw/tifileutil-rc.o
endif
