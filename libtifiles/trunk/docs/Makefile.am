## Process this file with automake to produce Makefile.in

EXTRA_DIST = api.txt

all:
	@echo "To build the documentation, run \"make docs\"."

docs:
if OS_WIN32
	tfdocgen.exe .
else
	tfdocgen .
endif

clean:
	rm -f html/*.c.html html/*.h.html html/api.html

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
