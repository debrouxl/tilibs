## Process this file with automake to produce Makefile.in

EXTRA_DIST = *.txt html
docdir = ${datadir}/doc/${PACKAGE}

all: docs

docs:
	tfdocgen ../

install:
	@INSTALL@ -d $(DESTDIR)${docdir}
	@INSTALL@ -d $(DESTDIR)${docdir}/html
	@INSTALL@ -m644 html/* $(DESTDIR)${docdir}/html
	@INSTALL@ -m644 ../AUTHORS ../ChangeLog ../COPYING ../README \
	$(DESTDIR)${docdir}
uninstall:
	rm -f $(DESTDIR)${docdir}/html/*

clean:
	rm -f html/*.c.html html/*.h.html html/api.html

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
