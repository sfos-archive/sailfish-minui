Name:       sailfish-minui
Summary:    Minimal UI enablers for Sailfish
Version:    0.0.1
Release:    1
Group:      System/Libraries
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-minui
Source0:    %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(minui)
BuildRequires: pkgconfig(sailfishsilica)

%description
%{summary}.

%package -n sailfish-mindbus-devel
Summary:    Minimal DBus C++ library
Group:      Development/Libraries

%description -n sailfish-mindbus-devel
%{summary}.

%package -n sailfish-minui-devel
Summary:    Minimal UI C++ library
Group:      Development/Libraries

%description -n sailfish-minui-devel
%{summary}.

%package -n sailfish-minui-dbus-devel
Summary:    Minimal UI C++ DBus event loop library
Group:      Development/Libraries

%description -n sailfish-minui-dbus-devel
%{summary}.

%package label-tool
Summary:    Label graphic generator tool for minui.
Group:      Development/Libraries
Requires:   sailfish-svg2png
Requires:   qt5-plugin-platform-offscreen

%description label-tool
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

%files -n sailfish-minui-devel
%defattr(-,root,root,-)
%dir %{_includedir}/sailfish-minui
%{_includedir}/sailfish-minui/eventloop.h
%{_includedir}/sailfish-minui-dbus/eventloop.h
%{_includedir}/sailfish-minui/ui.h
%{_libdir}/libsailfish-minui.a
%{_libdir}/pkgconfig/sailfish-minui.pc

%files -n sailfish-minui-dbus-devel
%defattr(-,root,root,-)
%dir %{_includedir}/sailfish-minui-dbus
%{_includedir}/sailfish-minui-dbus/eventloop.h
%{_libdir}/libsailfish-minui-dbus.a
%{_libdir}/pkgconfig/sailfish-minui-dbus.pc

%files label-tool
%defattr(-,root,root,-)
%{_bindir}/sailfish-minui-label-tool
%{_datadir}/qt5/mkspecs/features/sailfish-minui-resources.prf
