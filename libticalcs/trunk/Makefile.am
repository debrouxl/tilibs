## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SRCDIR = src12
if LIBGLIB2
 SRCDIR = src
endif
SUBDIRS = . docs macros man $(SRCDIR) tests win32 po intl

# Script to install
#bin_SCRIPTS = ticalcs-config

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO docs osx src12

pkgconfigdir = $(libdir)/pkgconfig
if OS_BSD
 pkgconfigdir = $(prefix)/libdata/pkgconfig
else
 pkgconfigdir = $(libdir)/pkgconfig
endif

pkgconfig_DATA = ticalcs.pc
if LIBGLIB2
 pkgconfig_DATA = ticalcs-4.pc
else
 pkgconfig_DATA = ticalcs.pc
 bin_SCRIPTS = ticalcs-config
endif

$(pkgconfig_DATA): config.status

dos2unix:
	dos2unix
	lowercase
	uppercase
	cd src; ${MAKE} dos2unix

#distclean:
#	rm -f *.log *.scan log

# This target retrieve some files from my Win$ partition to my Linux one
SRC_DIR=/mnt/xp/devel/tilp_project/libs/calcs
DST_DIR=/home/devel/tilp_project/libs/calcs

win2lin: $(C_FILES)
	@echo Merging files from Win32 to Linux...
	@echo dir1
	@for I in $(SRC_DIR)/src/*.c ; do \
	cp $$I $(DST_DIR)/src; \
	done
	@for I in $(SRC_DIR)/src/*.h ; do \
	cp $$I $(DST_DIR)/src; \
	done
	cd $(DST_DIR)/src; dos2unix; lowercase; uppercase;
	@echo dir2
	@for I in $(SRC_DIR)/tests/*.c ; do \
	cp $$I $(DST_DIR)/tests; \
	done
	cd $(DST_DIR)/tests; dos2unix; lowercase; uppercase;
	@echo dir3
	@for I in $(SRC_DIR)/win32/msvc/*.* ; do \
	cp $$I $(DST_DIR)/win32/msvc; \
	done
	@echo dir4
	@for I in $(SRC_DIR)/tests/*.dll ; do \
	cp $$I $(DST_DIR)/tests; \
	done
	@echo Done