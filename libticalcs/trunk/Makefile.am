## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS=-I m4

# Subdirectories to scan
SUBDIRS = build/mingw po src tests

if USE_DOCGEN
  SUBDIRS += docs
endif

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticalcs2.pc

