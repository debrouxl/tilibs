## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES=Makefile.in

EXTRA_DIST = ticables.m4

files:
        @files=`ls $(DISTFILES) 2> /dev/null `; for p in $$files; do \
          echo $$p; \
        done

aclocaldir = @ACLOCALDIR@

if Aclocal
 aclocal_DATA = ticables.m4
endif
