## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = . build docs intl macros man po src tests

# Script to install
bin_SCRIPTS = ticables-config

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO

pkgconfigdir = $(libdir)/pkgconfig
if OS_BSD
 pkgconfigdir = $(prefix)/libdata/pkgconfig
else
 pkgconfigdir = $(libdir)/pkgconfig
endif
pkgconfig_DATA = ticables.pc

$(pkgconfig_DATA): config.status

dos2unix:
	dos2unix
	lowercase
	uppercase
	cd src; ${MAKE} dos2unix

clean-local:
	rm -f log
