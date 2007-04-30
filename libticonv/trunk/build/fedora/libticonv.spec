Name: libticonv
Epoch: 1
Version: 1.0.2
Release: 1
Vendor: LPG (http://lpg.ticalc.org)
Packager: Kevin Kofler <Kevin@tigcc.ticalc.org>
Source: %{name}-%{version}.tar.bz2
Group: System Environment/Libraries
License: GPL
BuildRequires: glib2-devel >= 2.6.0
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Summary: Library for handling TI link cables
%description
Library for handling TI link cables

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{epoch}:%{version}-%{release}
Requires: pkgconfig
Requires: glib2-devel >= 2.6.0
%description devel
This package contains the files necessary to develop
applications using the %{name} library.

%prep
%setup -n libticonv

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} --libdir=%{_libdir} --disable-nls
make

%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm -f $RPM_BUILD_ROOT%{_libdir}/libticonv.la

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_libdir}/libticonv.so.*

%files devel
%defattr(-, root, root)
/usr/include/tilp2
%{_libdir}/libticonv.so
%{_libdir}/pkgconfig/ticonv.pc

%changelog
* Mon Apr 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org> 1:1.0.2-1
Bump Epoch.
Use real version number instead of date.

* Mon Apr 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org>
Remove redundant explicit Requires.
Don't BuildRequire newer versions than actually needed.

* Mon Sep 25 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Split out -devel into separate subpackage.
Own /usr/include/tilp2 in -devel.
Use more efficient method to call ldconfig in post/postun.

* Thu Jul 20 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Libdir fixes for lib64 platforms.
Add Provides for future -devel subpackage.

* Fri Jun 16 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Remove redundant %%defattr at the end of %%files.

* Wed Jun 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update file list (stdints.h now numbered to avoid conflicts).
Don't delete stdints.h anymore.
Don't require libticables2 anymore.

* Wed May 24 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Don't package .la file (not needed under Fedora).
Make sure permissions are set correctly when building as non-root.

* Mon May 22 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Build debuginfo RPM.
Use the system-wide default RPM_OPT_FLAGS instead of my own.
Use BuildRoot recommended by the Fedora packaging guidelines.

* Sun May 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
First Fedora RPM.
