## Process this file with automake to produce Makefile.in

if OS_WIN32
 no_undefined = -no-undefined
endif

INCLUDES = -I$(top_srcdir)/src

noinst_PROGRAMS = test_ticables

test_ticables_LDFLAGS = $(no_undefined)
test_ticables_LDADD = $(top_builddir)/src/libticables.la
