## Process this file with automake to produce Makefile.in

man_MANS = ticalcs-config.1

EXTRA_DIST = $(man_MANS) cleaner.c manpage.txt

files:
        @files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
          echo $$p; \
        done

view: $(man_MANS)
	groff -Tascii -man $(man_MANS) | less

dist_win: $(man_MANS)
	groff -Tascii -man $(man_MANS) > manpage
	gcc cleaner.c -o cleaner
	./cleaner manpage
	dos2unix manpage.txt
