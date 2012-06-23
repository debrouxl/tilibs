## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src @GLIB_CFLAGS@

noinst_PROGRAMS = test_ticonv torture_ticonv

test_ticonv_LDADD = $(top_builddir)/src/libticonv.la @GLIB_LIBS@
torture_ticonv_LDADD = $(top_builddir)/src/libticonv.la @GLIB_LIBS@

TESTS = torture_ticonv
