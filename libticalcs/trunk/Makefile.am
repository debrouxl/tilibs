## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = intl po src tests

if USE_DOCGEN
SUBDIRS += docs
endif

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticalcs2.pc
