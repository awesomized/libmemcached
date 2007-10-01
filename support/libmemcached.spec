Summary: memcached C library and command line tools
Name: libmemcached
Version: 0.3
Release: 1
License: BSD
Group: System Environment/Libraries
URL: http://tangent.org/552/libmemcached.html

Packager: Jeff Fisher <guppy@techmonkeys.org>

Source: http://download.tangent.org/libmemcached-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
libmemcached is a C client library to the memcached server
(http://danga.com/memcached). It has been designed to be light on memory
usage, and provide full access to server side methods.

It also implements several command line tools:

memcat - Copy the value of a key to standard output
memflush - Flush the contents of your servers.
memrm - Remove a key(s) from the serrver.
memstat - Dump the stats of your servers to standard output
memslap - Generate testing loads on a memcached cluster

%prep
%setup -q

%configure

%build
%{__make} %{_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install  DESTDIR="%{buildroot}" AM_INSTALL_PROGRAM_FLAGS=""

%clean
%{__rm} -rf %{buildroot}

%files
%{_bindir}/memcat
%{_bindir}/memcp
%{_bindir}/memflush
%{_bindir}/memrm
%{_bindir}/memstat
%{_bindir}/memslap
%{_includedir}/libmemcached/memcached.h
%{_libdir}/libmemcached.a
%{_libdir}/libmemcached.la
%{_libdir}/libmemcached.so
%{_libdir}/libmemcached.so.0
%{_libdir}/libmemcached.so.0.0.0

%changelog
* Mon Oct  1 2007 Brian Aker <brian@tangent.org> - 0.3-1
- Added memslap

* Fri Sep 28 2007 Jeff Fisher <guppy@techmonkeys.org> - 0.2-1
- Initial package
