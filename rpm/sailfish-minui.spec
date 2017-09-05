Name:       sailfish-minui
Summary:    Minimal UI enablers for Sailfish
Version:    0.0.1
Release:    1
Group:      System/Libraries
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-minui
Source0:    %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(dbus-1)

%description
%{summary}.

%package -n sailfish-mindbus-devel
Summary:    Minimal DBus C++ library
Group:      Development/Libraries

%description -n sailfish-mindbus-devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 "VERSION=%{version}"
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%files -n sailfish-mindbus-devel
%defattr(-,root,root,-)
%dir %{_includedir}/sailfish-mindbus
%{_includedir}/sailfish-mindbus/connection.h
%{_includedir}/sailfish-mindbus/message.h
%{_includedir}/sailfish-mindbus/object.h
%{_includedir}/sailfish-mindbus/pendingcall.h
%{_includedir}/sailfish-mindbus/pointer.h
%{_includedir}/sailfish-mindbus/signal.h
%{_libdir}/libsailfish-mindbus.a
%{_libdir}/pkgconfig/sailfish-mindbus.pc
