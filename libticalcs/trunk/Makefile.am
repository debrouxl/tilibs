## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = build intl po src tests

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticables.pc

$(pkgconfig_DATA): config.status

dos2unix:
	dos2unix
	lowercase
	uppercase
	cd src; ${MAKE} dos2unix
