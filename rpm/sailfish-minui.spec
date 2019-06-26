Name:       sailfish-minui
Summary:    Minimal UI enablers for Sailfish
Version:    0.0.22
Release:    1
Group:      System/Libraries
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-minui
Source0:    %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(minui)
BuildRequires: pkgconfig(sailfishsilica)
BuildRequires: qt5-qmake
BuildRequires: qt5-qttools
BuildRequires: qt5-qttools-linguist
BuildRequires: sailfish-svg2png
BuildRequires: sailfish-fonts
BuildRequires: qt5-plugin-platform-offscreen

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

%package resources-z1.0
Summary:    Scale factor 1.0 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z1.0
%{summary}.

%files resources-z1.0
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.0/font.png
%{_datadir}/%{name}/images/z1.0/icon-m-backspace.png
%{_datadir}/%{name}/images/z1.0/icon-m-accept.png
%{_datadir}/%{name}/images/z1.0/icon-m-cancel.png
%{_datadir}/%{name}/images/z1.0/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z1.0/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z1.0/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z1.0
ln -sf %{_datadir}/%{name}/images/z1.0 %{_datadir}/%{name}/images/default

%package resources-z1.25
Summary:    Scale factor 1.25 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z1.25
%{summary}.

%files resources-z1.25
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.25/font.png
%{_datadir}/%{name}/images/z1.25/icon-m-backspace.png
%{_datadir}/%{name}/images/z1.25/icon-m-accept.png
%{_datadir}/%{name}/images/z1.25/icon-m-cancel.png
%{_datadir}/%{name}/images/z1.25/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z1.25/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z1.25/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z1.25
ln -sf %{_datadir}/%{name}/images/z1.25 %{_datadir}/%{name}/images/default

%package resources-z1.5
Summary:    Scale factor 1.5 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z1.5
%{summary}.

%files resources-z1.5
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.5/font.png
%{_datadir}/%{name}/images/z1.5/icon-m-backspace.png
%{_datadir}/%{name}/images/z1.5/icon-m-accept.png
%{_datadir}/%{name}/images/z1.5/icon-m-cancel.png
%{_datadir}/%{name}/images/z1.5/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z1.5/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z1.5/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z1.5
ln -sf %{_datadir}/%{name}/images/z1.5 %{_datadir}/%{name}/images/default

%package resources-z1.5-large
Summary:    Scale factor 1.5 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z1.5-large
%{summary}.

%files resources-z1.5-large
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.5-large/font.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-backspace.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-accept.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-cancel.png
%{_datadir}/%{name}/images/z1.5-large/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z1.5-large/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z1.5-large/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z1.5-large
ln -sf %{_datadir}/%{name}/images/z1.5-large %{_datadir}/%{name}/images/default

%package resources-z1.75
Summary:    Scale factor 1.75 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z1.75
%{summary}.

%files resources-z1.75
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.75/font.png
%{_datadir}/%{name}/images/z1.75/icon-m-backspace.png
%{_datadir}/%{name}/images/z1.75/icon-m-accept.png
%{_datadir}/%{name}/images/z1.75/icon-m-cancel.png
%{_datadir}/%{name}/images/z1.75/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z1.75/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z1.75/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z1.75
ln -sf %{_datadir}/%{name}/images/z1.75 %{_datadir}/%{name}/images/default

%package resources-z2.0
Summary:    Scale factor 2.0 resources for the Sailfish MinUI
Group:      System/Applications
Requires(post): coreutils
Provides:   %{name}-resources

%description resources-z2.0
%{summary}.

%files resources-z2.0
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z2.0/font.png
%{_datadir}/%{name}/images/z2.0/icon-m-backspace.png
%{_datadir}/%{name}/images/z2.0/icon-m-accept.png
%{_datadir}/%{name}/images/z2.0/icon-m-cancel.png
%{_datadir}/%{name}/images/z2.0/graphic-busyindicator-medium-*.png
%{_datadir}/%{name}/images/z2.0/sailfish-minui-bt-*.png
%{_datadir}/%{name}/images/z2.0/sailfish-minui-la-*.png
%ghost %{_datadir}/%{name}/images/default

%post resources-z2.0
ln -sf %{_datadir}/%{name}/images/z2.0 %{_datadir}/%{name}/images/default

%package gallery
Summary:    Preview application for Sailfish MinUI toolkit components
Group:      System/Applications
Requires:   %{name}-gallery-resources

%description gallery
%{summary}.

%files gallery
%defattr(-,root,root,-)
%{_bindir}/sailfish-minui-gallery

%package gallery-resources-z1.0
Summary:    Scale factor 1.0 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z1.0

%description gallery-resources-z1.0
%{summary}.

%files gallery-resources-z1.0
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.0/icon-m-back.png
%{_datadir}/%{name}/images/z1.0/icon-m-clear.png
%{_datadir}/%{name}/images/z1.0/icon-m-dialpad.png
%{_datadir}/%{name}/images/z1.0/icon-m-gesture.png
%{_datadir}/%{name}/images/z1.0/icon-m-left.png
%{_datadir}/%{name}/images/z1.0/icon-m-menu.png
%{_datadir}/%{name}/images/z1.0/icon-m-right.png
%{_datadir}/%{name}/images/z1.0/sailfish-minui-gallery-*.png

%package gallery-resources-z1.25
Summary:    Scale factor 1.25 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z1.25

%description gallery-resources-z1.25
%{summary}.

%files gallery-resources-z1.25
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.25/icon-m-back.png
%{_datadir}/%{name}/images/z1.25/icon-m-clear.png
%{_datadir}/%{name}/images/z1.25/icon-m-dialpad.png
%{_datadir}/%{name}/images/z1.25/icon-m-gesture.png
%{_datadir}/%{name}/images/z1.25/icon-m-left.png
%{_datadir}/%{name}/images/z1.25/icon-m-menu.png
%{_datadir}/%{name}/images/z1.25/icon-m-right.png
%{_datadir}/%{name}/images/z1.25/sailfish-minui-gallery-*.png

%package gallery-resources-z1.5
Summary:    Scale factor 1.5 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z1.5

%description gallery-resources-z1.5
%{summary}.

%files gallery-resources-z1.5
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.5/icon-m-back.png
%{_datadir}/%{name}/images/z1.5/icon-m-clear.png
%{_datadir}/%{name}/images/z1.5/icon-m-dialpad.png
%{_datadir}/%{name}/images/z1.5/icon-m-gesture.png
%{_datadir}/%{name}/images/z1.5/icon-m-left.png
%{_datadir}/%{name}/images/z1.5/icon-m-menu.png
%{_datadir}/%{name}/images/z1.5/icon-m-right.png
%{_datadir}/%{name}/images/z1.5/sailfish-minui-gallery-*.png

%package gallery-resources-z1.5-large
Summary:    Scale factor 1.5 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z1.5-large

%description gallery-resources-z1.5-large
%{summary}.

%files gallery-resources-z1.5-large
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.5-large/icon-m-back.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-clear.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-dialpad.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-gesture.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-left.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-menu.png
%{_datadir}/%{name}/images/z1.5-large/icon-m-right.png
%{_datadir}/%{name}/images/z1.5-large/sailfish-minui-gallery-*.png

%package gallery-resources-z1.75
Summary:    Scale factor 1.75 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z1.75

%description gallery-resources-z1.75
%{summary}.

%files gallery-resources-z1.75
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z1.75/icon-m-back.png
%{_datadir}/%{name}/images/z1.75/icon-m-clear.png
%{_datadir}/%{name}/images/z1.75/icon-m-dialpad.png
%{_datadir}/%{name}/images/z1.75/icon-m-gesture.png
%{_datadir}/%{name}/images/z1.75/icon-m-left.png
%{_datadir}/%{name}/images/z1.75/icon-m-menu.png
%{_datadir}/%{name}/images/z1.75/icon-m-right.png
%{_datadir}/%{name}/images/z1.75/sailfish-minui-gallery-*.png

%package gallery-resources-z2.0
Summary:    Scale factor 2.0 resources for the Sailfish MinUI
Group:      System/Applications
Provides:   %{name}-gallery-resources
Requires:   %{name}-resources-z2.0

%description gallery-resources-z2.0
%{summary}.

%files gallery-resources-z2.0
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/images/z2.0/icon-m-back.png
%{_datadir}/%{name}/images/z2.0/icon-m-clear.png
%{_datadir}/%{name}/images/z2.0/icon-m-dialpad.png
%{_datadir}/%{name}/images/z2.0/icon-m-gesture.png
%{_datadir}/%{name}/images/z2.0/icon-m-left.png
%{_datadir}/%{name}/images/z2.0/icon-m-menu.png
%{_datadir}/%{name}/images/z2.0/icon-m-right.png
%{_datadir}/%{name}/images/z2.0/sailfish-minui-gallery-*.png

%package ts-devel
Summary:   Translation source for Sailfish MinUI
Group:     System/Applications

%description ts-devel
%{summary}.

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish-minui-gallery.ts
%{_datadir}/translations/source/sailfish-minui.ts
