#
# Template
#

TEMPLATE	= lib
LANGUAGE	= C

CONFIG -= qt
CONFIG	+= warn_on debug

HEADERS	+= src/*.h
SOURCES	+= src/*.c

QMAKE_PROJECT_DEPTH=1

unix:OBJECTS_DIR = .obj

#
# Dependancies and Version checking
#

GLIB2_MINVERSION = 2.6.0
HAVE_GLIB2 = $$system(pkg-config --atleast-version=$$GLIB2_MINVERSION glib-2.0 && echo yes || echo no)
!equals(HAVE_GLIB2,yes):error(glib2 $$GLIB2_MINVERSION or higher required.)
PKGCONFIG_CFLAGS += $$system(pkg-config --cflags glib-2.0)
LIBS += $$system(pkg-config --libs glib-2.0)

#
# Doc generation
#
exists(/usr/local/bin/tfdocgen) {
  message("Generating html documentation...")
#  unix  { tfdocgen     ../ }
#  win32 { tfdocgen.exe ../ }
}

#
# Path settings
#

PREFIX = $$(PREFIX)
isEmpty(PREFIX) {
  PREFIX = /usr/local
}

target.path = $$PREFIX/bin

pkgdata.path = $$PREFIX/share/tilp2
pkgdata.files =

docs.path = $$PREFIX/share/doc/ticonv
docs.files = html/*

INSTALLS += pkgdata docs target

#
# Various flags
#

VERSION = 1.0.0
PACKAGE = libticonv

LIBS	+= -Wl,--export-dynamic

linux-* { ARCH = -D__LINUX__ }
else *bsd-* { ARCH = -D__BSD__ }
CFLAGS += $$ARCH -Wno-unused

isEmpty(CFLAGS) {
  debug {
    CFLAGS = -Os -g
  } else {
    CFLAGS = -Os -s -fomit-frame-pointer
  }
}
QMAKE_CFLAGS_DEBUG = $$CFLAGS $$PKGCONFIG_CFLAGS -DSHARE_DIR='"'"$${pkgdata.path}"'"' -DVERSION='"$$VERSION"' -DPACKAGE='"$$PACKAGE"'
QMAKE_CFLAGS_RELEASE = $$CFLAGS $$PKGCONFIG_CFLAGS -DSHARE_DIR='"'"$${pkgdata.path}"'"' -DVERSION='"$$VERSION"' -DPACKAGE='"$$PACKAGE"'

QMAKE_LFLAGS_RELEASE = -s

#
# Distribution
#

DISTFILES += $${pkgdata.files} $${docs.files} build/mingw/* build/msvc/* \
  AUTHORS ChangeLog COPYING INSTALL LOGO README

distbz2.target = dist-bzip2
distbz2.commands = zcat gfm.tar.gz | bzip2 --best -c > gfm.tar.bz2
distbz2.depends = dist

QMAKE_EXTRA_UNIX_TARGETS += distbz2
