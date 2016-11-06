## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@ @TICONV_CFLAGS@ @TICALCS_CFLAGS@

EXTRA_DIST = vcheck

noinst_PROGRAMS = test_tiopers torture_tiopers

test_tiopers_LDADD = $(top_builddir)/src/libtiopers.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@ @TICALCS_LIBS@

torture_tiopers_LDADD = $(top_builddir)/src/libtiopers.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@ @TICALCS_LIBS@

TESTS = torture_tiopers
