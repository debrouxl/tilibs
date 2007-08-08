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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# Written by Marcel Telka <marcel@telka.sk>, 2003.
#

EXTRA_DIST = dhahelper.c dhahelper.rc ReadMe.txt dhahelper.bat dhasetup.bat dhasetup.dsp dhasetup.dsw SOURCES

if OS_WIN32
noinst_PROGRAMS = dhasetup
dhasetup_SOURCES = dhasetup.c dhahelper.h
noinst_SCRIPTS = dhahelper.sys
else
EXTRA_DIST += dhasetup.c dhahelper.h
endif

dhahelper.$(OBJEXT): $(srcdir)/dhahelper.c $(srcdir)/dhahelper.h
	$(CC) -Wall -Os -c $(srcdir)/dhahelper.c -o dhahelper.$(OBJEXT)

base.tmp: dhahelper.$(OBJEXT)
	$(CC) 	-Wl,--base-file,base.tmp \
		-Wl,--entry,_DriverEntry@8 \
		-nostartfiles -nostdlib \
		-o junk.tmp \
		dhahelper.$(OBJEXT) \
		-lntoskrnl
	-rm -f junk.tmp

temp.exp: base.tmp
	$(DLLTOOL) --dllname dhahelper.sys --base-file base.tmp --output-exp temp.exp

dhahelper.sys: dhahelper.$(OBJEXT) temp.exp
	$(CC)	-Wl,--subsystem,native \
		-Wl,--image-base,0x10000 \
		-Wl,--file-alignment,0x1000 \
		-Wl,--section-alignment,0x1000 \
		-Wl,--entry,_DriverEntry@8 \
		-Wl,temp.exp \
		-mdll -nostartfiles -nostdlib \
		-o dhahelper.sys \
		dhahelper.$(OBJEXT) \
		-lntoskrnl
	$(STRIP) dhahelper.sys

CLEANFILES = dhahelper.sys dhahelper.$(OBJEXT) base.tmp temp.exp
