## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES=Makefile.in

EXTRA_DIST = ticalcs.m4

files:
        @files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
          echo $$p; \
        done

aclocaldir = @ACLOCALDIR@

if Aclocal
 aclocal_DATA = ticalcs.m4
endif
