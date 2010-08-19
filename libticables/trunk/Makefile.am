## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS=-I m4

# Subdirectories to scan
SUBDIRS = build/mingw po src tests

if USE_DOCGEN
  SUBDIRS += docs
endif

# Extra files to add to the (distributed) package
EXTRA_DIST = CONFIG LOGO build

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ticables2.pc

install-exec-local:
if OS_WIN32
	@echo '------------------------------------------------------------------------------'
	@echo 'To install drivers, please run:'
	@echo 'BlackLink, ParallelLink on Win32: dhasetup install'
	@echo 'BlackLink, ParallelLink on Win64: rwpsetup install'
	@echo 'USB: rundll32 libusb0.dll,usb_install_driver_np_rundll DRIVER where DRIVER is:'
	@echo '  SilverLink: $(pkgdatadir)/libusb/silverlk.inf'
	@echo '  DirectLink TI-89 Titanium: $(pkgdatadir)/libusb/titanium.inf'
	@echo '  DirectLink TI-84+: $(pkgdatadir)/libusb/ti84plus.inf'
	@echo '  DirectLink TI-84+ SE: $(pkgdatadir)/libusb/ti84pse.inf'
	@echo '  DirectLink TI-Nspire: $(pkgdatadir)/libusb/nspire.inf'
	@echo '------------------------------------------------------------------------------'
endif
