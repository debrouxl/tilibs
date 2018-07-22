## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src @GLIB_CFLAGS@

noinst_PROGRAMS = test_ticonv torture_ticonv

test_ticonv_SOURCES = test_ticonv.cc
test_ticonv_LDADD = $(top_builddir)/src/libticonv.la @GLIB_LIBS@
torture_ticonv_SOURCES = torture_ticonv.c
torture_ticonv_LDADD = $(top_builddir)/src/libticonv.la @GLIB_LIBS@

EXTRA_DIST = test_ticonv.cc torture_ticonv.c

TESTS = torture_ticonv
