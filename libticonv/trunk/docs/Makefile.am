## Process this file with automake to produce Makefile.in

EXTRA_DIST = api.txt

all: docs

docs:
if OS_WIN32
	tfdocgen.exe ../
else
	tfdocgen ../
endif

install:
	@INSTALL@ -d $(DESTDIR)${docdir}
	@INSTALL@ -d $(DESTDIR)${docdir}/charsets
	@INSTALL@ -m555 charsets/* $(DESTDIR)${docdir}/charsets
	@INSTALL@ -d $(DESTDIR)${docdir}/html
	@INSTALL@ -m555 html/* $(DESTDIR)${docdir}/html

uninstall:
	rm -f $(DESTDIR)${docdir}/charsets/*
	rm -f $(DESTDIR)${docdir}/html/*

clean:
	rm -f html/*.c.html html/*.h.html html/api.html

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
