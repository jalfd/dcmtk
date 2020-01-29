# We always want debuginfo available because we always analyze crashes
# on-site because coredumps are not PHI-clean. The easiest way to achive this is
# to follow the style from easyviz.spec and avoid building debuginfo rpms altogether.
%define __strip /bin/true
%define debug_package %{nil}

Name: evdcmtk-3.6.4
Summary: Offis DICOM Toolkit (DCMTK)
# The version number is not used.
Version: 1
Release: 13%{?dist}
License: BSD
Group: Karos Health/EasyViz
Source: dcmtk-3.6.4.tar.gz
Prefix: /opt/easyviz/dcmtk-3.6.4

BuildRequires: cmake
BuildRequires: libjpeg-turbo-devel
BuildRequires: libpng-devel
BuildRequires: libtiff-devel
BuildRequires: libxml2-devel
BuildRequires: openssl-devel
BuildRequires: zlib-devel
BuildRequires: libicu-devel
BuildRequires: doxygen
%if 0%{?rhel} && 0%{?rhel} <= 7
BuildRequires: devtoolset-8-toolchain
Requires: devtoolset-8-runtime
%endif
Requires: libjpeg-turbo
Requires: libpng
Requires: libtiff
Requires: libxml2
Requires: openssl-libs
Requires: zlib
Requires: libicu

%description
DCMTK is a collection of libraries and applications implementing large
parts the DICOM standard. It includes software for examining,
constructing and converting DICOM image files, handling offline media,
sending and receiving images over a network connection, as well as
demonstrative image storage and worklist servers. DCMTK is is written
in a mixture of ANSI C and C++.  It comes in complete source code and
is made available as "open source" software. This package includes
minor adaptations to support the needs of the EasyViz diagnostic viewer.

%package devel
Summary: Development Libraries and Headers for dcmtk
Group: Development/Libraries
Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: libpng-devel%{?_isa}
Requires: libtiff-devel%{?_isa}
Requires: libicu-devel%{?_isa}
Requires: libxml2-devel%{?_isa}

%description devel
Development Libraries and Headers for dcmtk.  You only need to install
this if you are developing programs that use the dcmtk libraries.

%prep
%setup -q -n dcmtk-3.6.4

%build
%if 0%{?rhel} && 0%{?rhel} <= 7
if [[ ! $X_SCLS =~ "devtoolset-8" ]]; then
    set +x
    echo
    echo 'This package must be built with devtoolset-8 on RHEL/CentOS 7.'
    echo 'Please run "scl enable devtoolset-8 bash" before running rpmbuild.'
    false
fi
%endif

%define _prefix  /opt/easyviz/dcmtk-3.6.4

%cmake -DCMAKE_BUILD_TYPE:STRING="RelWithDebInfo" \
        -DCMAKE_INSTALL_PREFIX=%{_prefix} \
        -DCMAKE_INSTALL_RPATH=%{_libdir} \
	-DBUILD_APPS=ON \
	-DBUILD_SHARED_LIBS=ON \
        -DDCMTK_ENABLE_STL=ON \
        -DDCMTK_ENABLE_BUILTIN_DICTIONARY=ON \
        -DDCMTK_ENABLE_EXTERNAL_DICTIONARY=ON \
        -DDCMTK_ENABLE_PRIVATE_TAGS=ON \
	-DDCMTK_WITH_OPENSSL=ON \
	-DDCMTK_WITH_PNG=ON \
	-DDCMTK_WITH_TIFF=ON \
	-DDCMTK_WITH_XML=ON \
	-DDCMTK_WITH_DOXYGEN=ON \
	-DDCMTK_WITH_THREADS=ON \
        -DDCMTK_WITH_WRAP=FALSE \
        .

make %{?_smp_mflags}

%install
make DESTDIR=$RPM_BUILD_ROOT install

#Move configuration file from /usr/etc to /etc/
#mv $RPM_BUILD_ROOT/usr/etc $RPM_BUILD_ROOT

# Move the dicom dictionary to the documentation directory.
# We have the dictionary compiled in so reading it is a noop anyway.
mv $RPM_BUILD_ROOT/%{_prefix}/share/dcmtk/dicom.dic $RPM_BUILD_ROOT/%{_prefix}/share/doc/dcmtk/dicom.dic

# Remove zero-length file
rm $RPM_BUILD_ROOT/%{_datadir}/dcmtk/wlistdb/OFFIS/lockfile

%files
%dir %{_prefix}/%{_sysconfdir}/dcmtk/
%dir %{_datarootdir}/dcmtk
%dir %{_docdir}/dcmtk/
%docdir %{_docdir}/dcmtk/
%{_docdir}/dcmtk/*
%{_bindir}/*
%{_libdir}/*.so.*
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/dcmpstat.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/dcmqrscp.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/printers.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/storescp.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/storescu.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/filelog.cfg
%config(noreplace) %{_prefix}/%{_sysconfdir}/dcmtk/logger.cfg

%{_datadir}/dcmtk/*
%{_mandir}/man1/*


%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/cmake/dcmtk/

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%changelog
* Thu Aug 22 2019 Jesper Bojesen <jbojesen@vitalimages.com> - 3.6.4-1.13
- EV-5237: Charls decompressed color images are always non-planar RGB

* Thu Aug 15 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-12
- EV-5201: Disable Nagle's algorithm by default

* Thu Jul 18 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-11
- EV-4840: Full rename to dcmcharls that also updates DCMTKConfig.cmake

* Thu Jul 18 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-10
- EV-5110: Don't use environment variables to configure DCMTK
- EV-4840: Rename charls to dcmcharls to avoid tripping up the Windows
  installer

* Mon Jul 08 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-9
- EV-5038: Remove check for template identifier on containers only

* Tue Jun 18 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-8
- EV-4954: Ensure that VR=UN sequences are read as LEI

* Wed Mar 06 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-7
- EV-4533: Reintroduce old DcmElementList patch from EasyViz dcmtk-cvs to make it easier to
  access dcmdataset from multiple threads

* Mon Jan 28 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-6
- EV-1080: Fix missing Require for libxml2-devel, despite libxml2 only being used internally.
  The CMake build system in dcmtk is unfortunately broken and all dependencies are public.

* Mon Jan 28 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-5
- EV-1080: Add missing Requires and fix BuildRequires

* Mon Jan 28 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-4
- EV-4421: Build with devtoolset-8 on RHEL/CentOS

* Mon Jan 28 2019 Thomas Sondergaard <tsondergaard@vitalimages.com> - 3.6.4-1-3
- Keep debug symbols embedded to ensure they are available for easyviz-coredump

* Mon Jan 21 2019 Jesper Bojesen <jbojesen@vitalimages.com>
- 3.6.4-1 Import from Offis and port EasyViz modification from 3.5.4
