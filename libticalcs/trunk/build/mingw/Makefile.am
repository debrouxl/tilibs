EXTRA_DIST = ticalcs2.rc

if OS_WIN32
  noinst_DATA = ticalcs2-rc.o
endif

if OS_WIN32
all: ticalcs2-rc.o

ticalcs2-rc.o : ticalcs2.rc
	windres.exe ticalcs2.rc $@
endif