Name:       sailfish-minui
Summary:    Minimal UI enablers for Sailfish
Version:    0.0.24
Release:    1
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-minui
Source0:    %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(minui)
BuildRequires: pkgconfig(sailfishsilica)
BuildRequires: qt5-qmake
BuildRequires: qt5-qttools
BuildRequires: qt5-qttools-linguist
BuildRequires: sailfish-svg2png >= 0.3.4
BuildRequires: sailfish-fonts
BuildRequires: qt5-plugin-platform-offscreen

%description
%{summary}.

%package -n sailfish-mindbus-devel
Summary:    Minimal DBus C++ library

%description -n sailfish-mindbus-devel
%{summary}.

%package -n sailfish-minui-devel
Summary:    Minimal UI C++ library

%description -n sailfish-minui-devel
%{summary}.

%package -n sailfish-minui-dbus-devel
Summary:    Minimal UI C++ DBus event loop library

%description -n sailfish-minui-dbus-devel
%{summary}.

%package label-tool
Summary:    Label graphic generator tool for minui.
Requires:   sailfish-svg2png
Requires:   sailfish-fonts
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
mkdir -p %{buildroot}%{_datadir}/%{name}/images
touch %{buildroot}%{_datadir}/%{name}/images/default

%files -n sailfish-mindbus-devel
%defattr(-,root,root,-)
%dir %{_includedir}/sailfish-mindbus
%{_includedir}/sailfish-mindbus/*.h
%{_libdir}/libsailfish-mindbus.a
%{_libdir}/pkgconfig/sailfish-mindbus.pc

%files -n sailfish-minui-devel
%defattr(-,root,root,-)
%dir %{_includedir}/sailfish-minui
%{_includedir}/sailfish-minui/*.h
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

%package gallery
Summary:    Preview application for Sailfish MinUI toolkit components
Requires:   %{name}-gallery-resources

%description gallery
%{summary}.

%files gallery
%defattr(-,root,root,-)
%{_bindir}/sailfish-minui-gallery

%define sailfish_content_graphics_for_scale(z:d:) \
%package resources-%{-z*} \
Summary:    Scale factor %{-d*} resources for the Sailfish MinUI \
Requires\(post\): coreutils \
Provides:   %{name}-resources \
\
%description resources-%{-z*} \
%{summary}. \
\
%files resources-%{-z*} \
%defattr(-,root,root,-) \
%dir %{_datadir}/%{name} \
%{_datadir}/%{name}/images/%{-z*}/font.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-backspace.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-accept.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-cancel.png \
%{_datadir}/%{name}/images/%{-z*}/graphic-busyindicator-medium-*.png \
%{_datadir}/%{name}/images/%{-z*}/sailfish-minui-bt-*.png \
%{_datadir}/%{name}/images/%{-z*}/sailfish-minui-la-*.png \
%ghost %{_datadir}/%{name}/images/default \
\
%post resources-%{-z*} \
ln -sf %{_datadir}/%{name}/images/%{-z*} %{_datadir}/%{name}/images/default \
\
%package gallery-resources-%{-z*} \
Summary:    Scale factor %{-d*} resources for the Sailfish MinUI \
Provides:   %{name}-gallery-resources \
Requires:   %{name}-resources-%{-z*} \
\
%description gallery-resources-%{-z*} \
%{summary}. \
\
%files gallery-resources-%{-z*} \
%defattr(-,root,root,-) \
%dir %{_datadir}/%{name} \
%{_datadir}/%{name}/images/%{-z*}/icon-m-back.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-clear.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-dialpad.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-gesture.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-left.png\
%{_datadir}/%{name}/images/%{-z*}/icon-m-menu.png \
%{_datadir}/%{name}/images/%{-z*}/icon-m-right.png \
%{_datadir}/%{name}/images/%{-z*}/sailfish-minui-gallery-*.png

%sailfish_content_graphics_for_each_scale

%package ts-devel
Summary:   Translation source for Sailfish MinUI

%description ts-devel
%{summary}.

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish-minui-gallery.ts
%{_datadir}/translations/source/sailfish-minui.ts
