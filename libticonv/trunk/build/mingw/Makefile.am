EXTRA_DIST = ticonv.rc

if OS_WIN32
  noinst_DATA = ticonv-rc.o
endif

if OS_WIN32
all: ticonv-rc.o

ticonv-rc.o : ticonv.rc
	windres.exe ticonv.rc $@
endif