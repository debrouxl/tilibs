## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall

if USE_ICONV
 BUILT_SOURCES = ticonv.h
 opt = iconv.c;
ticonv.h: Makefile
	echo "Use iconv !";
	sed -i -e 's/#undef TICONV_ICONV_INTERFACE/#define TICONV_ICONV_INTERFACE/g' ticonv.h;
else
 opt =
endif

# libraries and files to compile and install
lib_LTLIBRARIES = libticonv.la
libticonvincludedir = $(includedir)/tilp2
libticonvinclude_HEADERS = \
	ticonv.h export4.h charset.h stdints4.h

# build instructions
libticonv_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@GLIB_CFLAGS@ -D_DEBUG
libticonv_la_LIBADD = @GLIB_LIBS@
libticonv_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libticonv_la_SOURCES = \
	ticonv.c charset.c tokens.c filename.c $(opt)

#if USE_ICONV
#else
#sed -i -e 's/#define TICONV_ICONV_INTERFACE/#undef TICONV_ICONV_INTERFACE/g' ticonv.h
#endif
