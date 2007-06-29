## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = src tests

if OS_WIN32
SUBDIRS += build/mingw
endif

if USE_DOCGEN
SUBDIRS += docs
endif

EXTRA_DIST = LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticonv.pc
