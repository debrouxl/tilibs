## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

INCLUDES = -I$(top_srcdir)/src

noinst_PROGRAMS = test_tifiles

test_tifiles_LDFLAGS = $(no_undefined)
test_tifiles_LDADD = $(top_builddir)/src/libtifiles.la
