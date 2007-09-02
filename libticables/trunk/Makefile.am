## Process this file with automake to produce Makefile.in

# Subdirectories to scan
SUBDIRS = build/mingw po src tests

if USE_DOCGEN
  SUBDIRS += docs
endif

# Extra files to add to the (distributed) package
EXTRA_DIST = LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticables2.pc

install-exec-local:
if OS_WIN32
	echo 'To install drivers, please run:'
	echo 'BlackLink, ParallelLink on Win32: dhasetup install'
	echo 'BlackLink, ParallelLink on Win64: rwpsetup install'
	echo 'USB: rundll32 libusb0.dll,usb_install_driver_np_rundll DRIVER where DRIVER is:'
	echo '  SilverLink: $(libusbdir)/silverlk.inf'
	echo '  DirectLink TI-89 Titanium: $(libusbdir)/titanium.inf'
	echo '  DirectLink TI-84+: $(libusbdir)/ti84plus.inf'
	echo '  DirectLink TI-84+ SE: $(libusbdir)/ti84pse.inf'
endif
