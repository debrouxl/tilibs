## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS=-I m4

# subdirectories to scan
SUBDIRS = build po src

if USE_GROFF
  SUBDIRS += man
endif

dist_pkgdata_DATA = COPYING RELEASE

DISTCLEANFILES = ... intltool-extract intltool-merge intltool-update po/.intltool-merge-cache

EXTRA_DIST = README.* TODO

INTLTOOL_FILES = intltool-extract.in intltool-merge.in intltool-update.in
