Name: libticalcs2
Epoch: 1
Version: 1.0.8
Release: 1
Vendor: LPG (http://lpg.ticalc.org)
Packager: Kevin Kofler <Kevin@tigcc.ticalc.org>
Source: %{name}-%{version}.tar.bz2
#LANG=C svn diff -r 3886:3887 src/calc_89.c >../libticalcs2-1.0.8-fix-ti92p-v200-romdump.diff
Patch0: libticalcs2-1.0.8-fix-ti92p-v200-romdump.diff
Group: System Environment/Libraries
License: GPL
BuildRequires: libticables2-devel >= 1:1.0.6, libticonv-devel >= 1:1.0.4, libtifiles2-devel >= 1:1.0.7, glib2-devel >= 2.6.0, tfdocgen
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Summary: Library for handling TI calculators through a common API
%description
Library for handling TI calculators through a common API

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{epoch}:%{version}-%{release}
Requires: pkgconfig
Requires: libticables2-devel >= 1:1.0.6, libticonv-devel >= 1:1.0.4, libtifiles2-devel >= 1:1.0.7, glib2-devel >= 2.6.0
%description devel
This package contains the files necessary to develop applications using the
%{name} library.

%package apidocs
Summary: API documentation for %{name}
Group: Development/Documentation
Requires: %{name} = %{epoch}:%{version}-%{release}
%description apidocs
This package contains the API documentation for the %{name} library in
HTML format.

%prep
%setup
%patch0 -p0

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} --libdir=%{_libdir} --disable-nls
make

%install
if [ -d $RPM_BUILD_ROOT ]; then rm -rf $RPM_BUILD_ROOT; fi
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm -f $RPM_BUILD_ROOT%{_libdir}/libticalcs2.la

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_libdir}/libticalcs2.so.*
%dir %{_datadir}/doc/%{name}
%{_datadir}/doc/%{name}/AUTHORS
%{_datadir}/doc/%{name}/COPYING
%{_datadir}/doc/%{name}/ChangeLog
%{_datadir}/doc/%{name}/README

%files devel
%defattr(-, root, root)
%{_includedir}/tilp2
%{_libdir}/libticalcs2.so
%{_libdir}/pkgconfig/ticalcs2.pc

%files apidocs
%defattr(-, root, root)
%{_datadir}/doc/%{name}/html

%changelog
* Sat Oct 13 2007 Kevin Kofler <Kevin@tigcc.ticalc.org> 1:1.0.8-1
Update to 1.0.8.
Backport fix for TI-92+/V200 ROM dumps from SVN.

* Wed Jun 27 2007 Kevin Kofler <Kevin@tigcc.ticalc.org> 1:1.0.7-1
Update to 1.0.7.
Drop obsolete backported patch.
Update tilibs BuildRequires.

* Wed May 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org>
Drop -n libticalcs, the tarball uses name-version format now.
Add BR tfdocgen and apidocs subpackage.
Package non-API documentation files in main package.
Backport documentation generation and installation fix from SVN.

* Mon Apr 16 2007 Kevin Kofler <Kevin@tigcc.ticalc.org> 1:1.0.6-1
Bump Epoch.
Use real version number instead of date.
Also use real version numbers and Epoch 1 for the dependencies.

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
Use libtifoo-devel instead of libtifoo in BuildRequires.

* Fri Jun 16 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Remove redundant %%defattr at the end of %%files.

* Wed Jun 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update file list (stdints.h now numbered to avoid conflicts).

* Wed May 24 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Don't package .la file (not needed under Fedora).
Make sure permissions are set correctly when building as non-root.

* Mon May 22 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Build debuginfo RPM.
Use the system-wide default RPM_OPT_FLAGS instead of my own.
Use BuildRoot recommended by the Fedora packaging guidelines.

* Sun May 7 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
License now GPL.
Add missing glib2 BuildRequires/Requires.
Now requires libticonv.

* Sat Feb 11 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update setup -n to use the new directory name (libticalcs, not libticalcs2).

* Sun Jan 29 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Strip library because -s gets ignored somehow.

* Wed Jan 4 2006 Kevin Kofler <Kevin@tigcc.ticalc.org>
Change Vendor to LPG.

* Wed Sep 14 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
Update to libticalcs-2: new package name, updated file list and dependencies.

* Sun Jun 19 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
Change Copyright to License.

* Mon May 2 2005 Kevin Kofler <Kevin@tigcc.ticalc.org>
First Fedora RPM.
