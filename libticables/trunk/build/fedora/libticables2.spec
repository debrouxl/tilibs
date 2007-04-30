Name: libticables2
Epoch: 1
Version: 1.0.5
Release: 1
Vendor: LPG (http://lpg.ticalc.org)
Packager: Kevin Kofler <Kevin@tigcc.ticalc.org>
Source: %{name}-%{version}.tar.bz2
Group: System Environment/Libraries
License: GPL
BuildRequires: libusb-devel, glib2-devel >= 2.4.0
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Summary: Library for handling TI link cables
%description
Library for handling TI link cables

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{epoch}:%{version}-%{release}
Requires: pkgconfig
Requires: glib2-devel >= 2.4.0
%description devel
This package contains the files necessary to develop
applications using the %{name} library.

%prep
%setup -n libticables

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} --libdir=%{_libdir} --disable-nls
make

%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm -f $RPM_BUILD_ROOT%{_libdir}/libticables2.la
mkdir -p $RPM_BUILD_ROOT/etc/udev/rules.d
cat >$RPM_BUILD_ROOT/etc/udev/rules.d/60-libticables.rules <<EOF1
# This file was installed by the libticables2 Fedora package.

# SilverLink
ACTION=="add", SUBSYSTEM=="usb_device", SYSFS{idVendor}=="0451", SYSFS{idProduct}=="e001", SYMLINK+="ticable-%%k"
# TI-84+ DirectLink
ACTION=="add", SUBSYSTEM=="usb_device", SYSFS{idVendor}=="0451", SYSFS{idProduct}=="e003", SYMLINK+="ticable-%%k"
# TI-89 Titanium DirectLink
ACTION=="add", SUBSYSTEM=="usb_device", SYSFS{idVendor}=="0451", SYSFS{idProduct}=="e004", SYMLINK+="ticable-%%k"
# TI-84+ SE DirectLink
ACTION=="add", SUBSYSTEM=="usb_device", SYSFS{idVendor}=="0451", SYSFS{idProduct}=="e008", SYMLINK+="ticable-%%k"
EOF1
mkdir -p $RPM_BUILD_ROOT/etc/security/console.perms.d
cat >$RPM_BUILD_ROOT/etc/security/console.perms.d/60-libticables.perms <<EOF2
# This file was installed by the libticables2 Fedora package.

# device classes -- these are shell-style globs
<ticable>=/dev/ticable* /dev/usb/ticable*

# permission definitions
<console>  0600 <ticable>    0600 root
EOF2

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_libdir}/libticables2.so.*
/etc/udev/rules.d/60-libticables.rules
/etc/security/console.perms.d/60-libticables.perms

%files devel
%defattr(-, root, root)
/usr/include/tilp2
%{_libdir}/libticables2.so
%{_libdir}/pkgconfig/ticables2.pc

%changelog
* Mon Apr 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org> 1:1.0.5-1
Bump Epoch.
Use real version number instead of date.

* Mon Apr 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org>
Remove redundant explicit Requires.
Don't BuildRequire newer versions than actually needed.

* Sun Oct 15 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Add non-SE TI-84+ to the pam_console configuration.

* Mon Sep 25 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Split out -devel into separate subpackage.
Own /usr/include/tilp2 in -devel.
Use more efficient method to call ldconfig in post/postun.

* Sun Jul 23 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Fix incorrect escaping in console.perms.d file.

* Thu Jul 20 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Libdir fixes for lib64 platforms.
Add Provides for future -devel subpackage.

* Fri Jun 16 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Remove redundant %%defattr at the end of %%files.

* Wed Jun 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update file list (stdints.h now numbered to avoid conflicts).
Use /etc/security/console.perms.d for the pam_console setup instead of
hand-written script hack.

* Wed May 24 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Don't package .la file (not needed under Fedora).
Make sure permissions are set correctly when building as non-root.

* Mon May 22 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Build debuginfo RPM.
Use the system-wide default RPM_OPT_FLAGS instead of my own.
Use BuildRoot recommended by the Fedora packaging guidelines.

* Sun May 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
License now GPL (with exception for TilEm).
Add missing glib2 BuildRequires/Requires.
Now requires libticonv.
Convert hotplug rules to udev rules and add DirectLink DeviceIDs.

* Sat Feb 11 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update setup -n to use the new directory name (libticables, not libticables2).

* Sun Jan 29 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Strip library because -s gets ignored somehow.

* Wed Jan 4 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Change Vendor to LPG.

* Wed Sep 14 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update to libticables-2: new package name, updated file list.

* Wed Jun 22 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
SilverLink support (based on Julien Blache's Debian packages and Nalin
Dahyabhai's usbcam script)

* Sun Jun 19 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
Change Copyright to License.
Don't list cabl_def.h twice.

* Mon May 2 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
First Fedora RPM.
