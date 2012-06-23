## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@ @TICONV_CFLAGS@

EXTRA_DIST = ticalcs2.supp vcheck

noinst_PROGRAMS = test_ticalcs_2 torture_ticalcs

test_ticalcs_2_LDADD = $(top_builddir)/src/libticalcs2.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@

torture_ticalcs_LDADD = $(top_builddir)/src/libticalcs2.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@ @TICONV_LIBS@

TESTS = torture_ticalcs
