Procedure to set up the environment:

- get MinGW & MSys from <http://www.mingw.org/> and install it
- get pkg-config from <http://www.gimp.org/~tml/gimp/win32/downloads.html>
- pkg-config depends on: glib, glib-dev, libiconv, libgettext. Get them at same location.
- simply select folders and copy them into MSys.

Before running ./configure --disable-nls, set the PKG_CONFIG_PATH with:
export PKG_CONFIG_PATH="c:/msys/1.0/lib/pkgconfig".