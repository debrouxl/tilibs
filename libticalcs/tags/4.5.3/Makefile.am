## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = . docs macros man po src tests #win32 intl

# Script to install
bin_SCRIPTS = ticalcs-config

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO build docs

pkgconfigdir = $(libdir)/pkgconfig
if OS_BSD
 pkgconfigdir = $(prefix)/libdata/pkgconfig
else
 pkgconfigdir = $(libdir)/pkgconfig
endif
pkgconfig_DATA = ticalcs.pc

$(pkgconfig_DATA): config.status

dos2unix:
	dos2unix
	lowercase
	uppercase
	cd src; ${MAKE} dos2unix

clean-local:
	rm -f log
