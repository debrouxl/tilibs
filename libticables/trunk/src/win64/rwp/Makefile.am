# Makefile.am adapted from Cygwin ioperm
# http://openwince.sourceforge.net/ioperm/
#
# Copyright (C) 2003 ETC s.r.o.
# Copyright (C) 2007 Kevin Kofler
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
# Written by Marcel Telka <marcel@telka.sk>, 2003.
#

EXTRA_DIST = rwports.c rwports.rc common.ver ntverp.h ReadMe.txt rwports.bat rwpsetup.bat net.bat rwpsetup.dsp rwpsetup.dsw SOURCES NMAKEFILE

if OS_WIN32
bin_PROGRAMS = rwpsetup
rwpsetup_SOURCES = rwpsetup.c rwports.h
bin_SCRIPTS = rwports.sys
else
EXTRA_DIST += rwpsetup.c rwports.h
endif

# We will need a working x86_64-pc-mingw32 toolchain to build this.
#
#RC	?= windres
#
#rwports.$(OBJEXT): $(srcdir)/rwports.c $(srcdir)/rwports.h
#	$(CC) -Wall -Os -c $(srcdir)/rwports.c -o rwports.$(OBJEXT)
#
#rwports-rc.$(OBJEXT): $(srcdir)/rwports.rc $(srcdir)/common.ver $(srcdir)/ntverp.h
#	$(RC) -I. rwports.rc $@
#
#base.tmp: rwports.$(OBJEXT) rwports-rc.$(OBJEXT)
#	$(CC) 	-Wl,--base-file,base.tmp \
#		-Wl,--entry,_DriverEntry@8 \
#		-nostartfiles -nostdlib \
#		-o junk.tmp \
#		rwports.$(OBJEXT) rwports-rc.$(OBJEXT) \
#		-lntoskrnl
#	-rm -f junk.tmp
#
#temp.exp: base.tmp
#	$(DLLTOOL) --dllname rwports.sys --base-file base.tmp --output-exp temp.exp
#
#rwports.sys: rwports.$(OBJEXT) rwports-rc.$(OBJEXT) temp.exp
#	$(CC)	-Wl,--subsystem,native \
#		-Wl,--image-base,0x10000 \
#		-Wl,--file-alignment,0x1000 \
#		-Wl,--section-alignment,0x1000 \
#		-Wl,--entry,_DriverEntry@8 \
#		-Wl,temp.exp \
#		-mdll -nostartfiles -nostdlib \
#		-o rwports.sys \
#		rwports.$(OBJEXT) rwports-rc.$(OBJEXT) \
#		-lntoskrnl
#	$(STRIP) rwports.sys

rwports.sys: ../../../build/msvc/rwports.sys
	cp -f ../../../build/msvc/rwports.sys rwports.sys

CLEANFILES = rwports.sys rwports.$(OBJEXT) rwports-rc.$(OBJEXT) base.tmp temp.exp
