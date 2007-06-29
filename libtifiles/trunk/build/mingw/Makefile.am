EXTRA_DIST = tifiles2.rc

if OS_WIN32
  noinst_DATA = tifiles2-rc.o
endif

if OS_WIN32
all: tifiles2-rc.o

tifiles2-rc.o : tifiles2.rc
	windres.exe tifiles2.rc $@
endif