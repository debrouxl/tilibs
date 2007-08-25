The libusb-win32 package provides support for gcc/mingw but does not provide
any integration with MSyS. This is the goal of this folder.

Simply download the latest libusb-win32 package from http://libusb-win32.sourceforge.net/ and
uncompress it. Copy the folder where you are reading this documentation into the libusb-win32
package:

libusb-win32-0.1.12
 +- bin
 +- examples
 +- include
 +- inf
 +- lib
=> +- libusb-win32

Go to the libusb-win32 folder and run install.bat. That's all! Now, you have the libusb-win32
package useable with MinGW thru libusb-config and PkgConfig.

---
R. Liévin
