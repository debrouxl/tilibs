## Process this file with automake to produce Makefile.in

EXTRA_DIST = gen_doc.c

all:
#  gen_doc .

gen_doc: gen_doc
	gcc `pkg-config glib-2.0 --cflags --libs` gen_doc.c -o gen_doc

clean:
	rm -f ./text/*~ ./html/*.*

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
