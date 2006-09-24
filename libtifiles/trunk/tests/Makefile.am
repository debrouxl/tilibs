## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_PROGRAMS = test_tifiles_2

test_tifiles_2_CPPFLAGS = @GLIB_CFLAGS@  -fno-common
test_tifiles_2_LDADD = $(top_builddir)/src/libtifiles2.la @GLIB_LIBS@
