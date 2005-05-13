## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@

noinst_PROGRAMS = test_ticalcs_2

test_ticalcs_2_LDADD = $(top_builddir)/src/libticalcs2.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@
