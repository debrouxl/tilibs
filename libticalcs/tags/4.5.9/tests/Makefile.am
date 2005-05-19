## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src \
	@TICABLES_CFLAGS@ @TIFILES_CFLAGS@

noinst_PROGRAMS = test_ticalcs

test_ticalcs_LDADD = $(top_builddir)/src/libticalcs.la \
	@TICABLES_LIBS@ @TIFILES_LIBS@
