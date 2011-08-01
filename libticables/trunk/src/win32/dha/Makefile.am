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
# 02111-1307, USA.
#
# Written by Marcel Telka <marcel@telka.sk>, 2003.
#

EXTRA_DIST = dhahelper.c dhahelper.rc common.ver ntverp.h ReadMe.txt dhahelper.bat dhasetup.bat dhasetup.dsp dhasetup.dsw SOURCES NMAKEFILE

if OS_WIN32
bin_PROGRAMS = dhasetup
dhasetup_SOURCES = dhasetup.c dhahelper.h
bin_SCRIPTS = dhahelper.sys
else
EXTRA_DIST += dhasetup.c dhahelper.h
endif

RC	?= windres

dhahelper.$(OBJEXT): $(srcdir)/dhahelper.c $(srcdir)/dhahelper.h
	$(CC) -Wall -Os -c $(srcdir)/dhahelper.c -o dhahelper.$(OBJEXT)

dhahelper-rc.$(OBJEXT): $(srcdir)/dhahelper.rc $(srcdir)/common.ver $(srcdir)/ntverp.h
	$(RC) -I. dhahelper.rc $@

base.tmp: dhahelper.$(OBJEXT) dhahelper-rc.$(OBJEXT)
	$(CC) 	-Wl,--base-file,base.tmp \
		-Wl,--entry,_DriverEntry@8 \
		-nostartfiles -nostdlib \
		-o junk.tmp \
		dhahelper.$(OBJEXT) dhahelper-rc.$(OBJEXT) \
		-lntoskrnl
	-rm -f junk.tmp

temp.exp: base.tmp
	$(DLLTOOL) --dllname dhahelper.sys --base-file base.tmp --output-exp temp.exp

dhahelper.sys: dhahelper.$(OBJEXT) dhahelper-rc.$(OBJEXT) temp.exp
	$(CC)	-Wl,--subsystem,native \
		-Wl,--image-base,0x10000 \
		-Wl,--file-alignment,0x1000 \
		-Wl,--section-alignment,0x1000 \
		-Wl,--entry,_DriverEntry@8 \
		-Wl,temp.exp \
		-mdll -nostartfiles -nostdlib \
		-o dhahelper.sys \
		dhahelper.$(OBJEXT) dhahelper-rc.$(OBJEXT) \
		-lntoskrnl
	$(STRIP) dhahelper.sys

CLEANFILES = dhahelper.sys dhahelper.$(OBJEXT) dhahelper-rc.$(OBJEXT) base.tmp temp.exp
