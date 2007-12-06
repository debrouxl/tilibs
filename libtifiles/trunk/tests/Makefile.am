## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_PROGRAMS = test_tifiles_2

test_tifiles_2_CPPFLAGS = @GLIB_CFLAGS@
test_tifiles_2_LDADD = $(top_builddir)/src/libtifiles2.la @GLIB_LIBS@

dist-hook:
	if test ! -d .svn; then \
	rm -rf misc certs tig ti73 ti82 ti83 ti84p ti85 ti86 ti89 ti92; \
	fi
	rm -rf $(distdir)/test/misc \
		$(distdir)/test/certs \
		$(distdir)/test/tig \
		$(distdir)/test/ti73 \
		$(distdir)/test/ti82 \
		$(distdir)/test/ti83 \
		$(distdir)/test/ti84p \
		$(distdir)/test/ti85 \
		$(distdir)/test/ti86 \
		$(distdir)/test/ti89 \
		$(distdir)/test/ti92