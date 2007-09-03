#!/bin/sh

if [ -z $PREFIX ]
  then PREFIX=/mingw
fi

mkdir -p $PREFIX/bin
mkdir -p $PREFIX/include
mkdir -p $PREFIX/lib
mkdir -p $PREFIX/lib/pkgconfig

cp -f ../bin/libusb0.dll  $PREFIX/bin
cp -f ../bin/libusb0*.sys $PREFIX/bin
cp -f ../include/usb.h    $PREFIX/include
cp -f ../lib/gcc/libusb.a $PREFIX/lib

sed -e 's!/mingw!'"$PREFIX"'!g' libusb.pc >$PREFIX/lib/pkgconfig/libusb.pc
sed -e 's!/mingw!'"$PREFIX"'!g' libusb-config >$PREFIX/bin/libusb-config

chmod +x $PREFIX/bin/libusb-config
