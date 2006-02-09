## Process this file with automake to produce Makefile.in

EXTRA_DIST = *.sh *.awk

index:
	./gen_doc.sh

clean:
	rm -f ./text/*~ index ./html/*.*

files:
	@files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
	  echo $$p; \
	done
