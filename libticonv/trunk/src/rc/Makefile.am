EXTRA_DIST = \
	ticonv.rc \
	ticonv.rc.in

if OS_WIN32
  noinst_DATA = ticonv-rc.o
endif

ticonv-rc.o : ticonv.rc
	windres.exe ticonv.rc $@
