Name: evdcmtk-3.6.4
Summary: Offis DICOM Toolkit (DCMTK)
# The version number is not used.
Version: 1
Release: 2%{?dist}
License: BSD
Group: Karos Health/EasyViz
Source: dcmtk-3.6.4.tar.gz
Prefix: /opt/easyviz/dcmtk-3.6.4

BuildRequires: cmake
BuildRequires: libjpeg-devel
BuildRequires: libpng-devel
BuildRequires: libtiff-devel
BuildRequires: libxml2-devel
BuildRequires: openssl-devel
BuildRequires: zlib-devel
BuildRequires: libicu-devel
BuildRequires: doxygen
%if 0%{?rhel}
BuildRequires: devtoolset-7-toolchain
Requires: devtoolset-7-runtime
%endif

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

%description devel
Development Libraries and Headers for dcmtk.  You only need to install
this if you are developing programs that use the dcmtk libraries.

%prep
%setup -q -n dcmtk-3.6.4

%build

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
* Mon Jan 21 2019 Jesper Bojesen <jbojesen@vitalimages.com>
- 3.6.4-1 Import from Offis and port EasyViz modification from 3.5.4
