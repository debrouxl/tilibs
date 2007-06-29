EXTRA_DIST = ticables2.rc

if OS_WIN32
  noinst_DATA = ticables2-rc.o
endif

if OS_WIN32
all: ticables2-rc.o

ticables2-rc.o : ticables2.rc
	windres.exe ticables2.rc $@
endif