mkdir c:\msys\bin
mkdir c:\msys\include
mkdir c:\msys\lib
mkdir c:\msys\lib\pkgconfig


copy ..\bin\libusb0.dll  c:\msys\bin
copy ..\include\usb.h    c:\msys\include
copy ..\lib\gcc\libusb.a c:\msys\lib
copy    libusb.pc        c:\msys\lib\pkgconfig
copy    libusb-config    c:\msys\bin