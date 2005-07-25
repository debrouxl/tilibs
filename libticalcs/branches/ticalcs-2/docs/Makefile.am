## Process this file with automake to produce Makefile.in

EXTRA_DIST = gen_doc.c api.txt

all: gen_doc
	./gen_doc .

gen_doc: gen_doc.c
if OS_WIN32
	gcc `pkg-config.exe glib-2.0 --cflags --libs` -Wall gen_doc.c -o gen_doc.exe
else
	gcc `pkg-config glib-2.0 --cflags --libs` -Wall gen_doc.c -o gen_doc
endif

clean:
	rm -f html/*.c.html html/*.h.html html/api.html

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
