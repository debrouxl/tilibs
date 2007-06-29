## Process this file with automake to produce Makefile.in

SUBDIRS = rc

AM_CFLAGS = -Wall

# libraries and files to compile and install
lib_LTLIBRARIES = libticonv.la
libticonvincludedir = $(includedir)/tilp2
libticonvinclude_HEADERS = \
	ticonv.h export4.h charset.h stdints4.h

# build instructions
libticonv_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ -DTICONV_EXPORTS
libticonv_la_LIBADD = @GLIB_LIBS@
libticonv_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticonv_la_SOURCES = \
	ticonv.c charset.c tokens.c filename.c

if OS_WIN32
  libticonv_la_DEPENDENCIES = ./rc/ticonv-rc.o
  libticonv_la_LDFLAGS += -Wl,./rc/ticonv-rc.o
endif

if !USE_ICONV
install-data-hook:
	sed -e 's/#define TICONV_ICONV_INTERFACE/#undef TICONV_ICONV_INTERFACE/g' $(DESTDIR)$(prefix)/include/tilp2/ticonv.h >$(DESTDIR)$(prefix)/include/tilp2/ticonv.h.tmp && mv -f $(DESTDIR)$(prefix)/include/tilp2/ticonv.h.tmp $(DESTDIR)$(prefix)/include/tilp2/ticonv.h;
endif
