#!/bin/bash

NPROC=1
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
 NPROC=$(sysctl -n hw.physicalcpu)
elif [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
 NPROC=$(nproc)
fi

mkdir prefix
prefixpath="$(pwd)/prefix"
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${prefixpath}/lib/pkgconfig

if [ "$PREBUILDER" == "autotools" ]; then
    cd libticonv/trunk
    mkdir m4; autoreconf -ivf &
    cd ../../libtifiles/trunk
    mkdir m4; autoreconf -ivf &
    cd ../../tifileutil/trunk
    mkdir m4; autoreconf -ivf &
    cd ../../libticables/trunk
    mkdir m4; autoreconf -ivf &
    cd ../../libticalcs/trunk
    mkdir m4; autoreconf -ivf &
    wait
    cd ../../libticonv/trunk
    ./configure --prefix=${prefixpath}
    make -j${NPROC} check
    make -j${NPROC} install
    cd ../../libtifiles/trunk
    cd po; make libtifiles2.pot-update; make update-po; cd ..
    ./configure --prefix=${prefixpath}
    make -j${NPROC} check
    make -j${NPROC} install
    cd ../../tifileutil/trunk
    cd po; make tifileutil.pot-update; make update-po; cd ..
    ./configure --prefix=${prefixpath}
    make -j${NPROC} check
    make -j${NPROC} install
    cd ../../libticables/trunk
    cd po; make libticables2.pot-update; make update-po; cd ..
    ./configure --prefix=${prefixpath} --enable-logging --enable-libusb10
    make -j${NPROC} check
    make -j${NPROC} install
    cd ../../libticalcs/trunk
    cd po; make libticalcs2.pot-update; make update-po; cd ..
    ./configure --prefix=${prefixpath}
    make -j${NPROC} check
    make -j${NPROC} install

elif [ "$PREBUILDER" == "cmake" ]; then
    mkdir build && cd build
    if [ "$BUILDER" == "ninja" ]; then
        GPARAM="-GNinja"
    elif [ "$BUILDER" != "make" ]; then
        echo "Unsupported builder: ${BUILDER}"
        exit -1
    fi
    cmake ${GPARAM} -DCMAKE_INSTALL_PREFIX=${prefixpath} ..
    cmake --build . --target all
    cmake --build . --target check
    cmake --build . --target install
else
    echo "Unsupported prebuilder: ${PREBUILDER}"
    exit -1
fi
