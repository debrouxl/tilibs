## Process this file with automake to produce Makefile.in

EXTRA_DIST = gen_doc.c api.txt

all: gen_doc
	cp ./tmpl/*.* ./html
	./gen_doc .

gen_doc: gen_doc.c
	gcc `pkg-config glib-2.0 --cflags --libs` -Wall gen_doc.c -o gen_doc

clean:
	rm -f ./text/*~ ./html/*.*

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
