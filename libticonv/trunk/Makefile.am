## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS=-I m4

# Subdirectories to scan
SUBDIRS = build/mingw src tests

if USE_DOCGEN
  SUBDIRS += docs
endif

EXTRA_DIST = LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticonv.pc
