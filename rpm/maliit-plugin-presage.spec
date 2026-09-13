Name:       maliit-plugin-presage

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    Maliit plugin for text predictions using Presage
Version:    1.1
Release:    1
Group:      Applications/Text
License:    GPLv3
URL:        https://github.com/sailfish-keyboard/sailfishos-presage-predictor
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   jolla-keyboard
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(tinyxml2)
BuildRequires:  libpresage-devel
BuildRequires:  libmarisa-devel
BuildRequires:  hunspell-devel >= 1.5.1
Obsoletes:   presage-data

%description
Keyboard prediction plugin based on the Presage prediction engine

PackageName: Maliit Presage Plugin
Icon: https://raw.githubusercontent.com/sailfish-keyboard/presage/master/resources/presage.png
Categories:
  - Office
  - Keyboard

%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 

%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%post
systemctl-user restart maliit-server.service || true

%postun
systemctl-user restart maliit-server.service || true

%files
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/hu/mm/presagepredictor/libPresagePredictor.so
%{_libdir}/qt5/qml/hu/mm/presagepredictor/qmldir
%if 0%{?sailfishos_version} >= 52000
%define _handler_datadir %{_datadir}/maliit/plugins/com/jolla/handlers
%else
%define _handler_datadir %{_datadir}/maliit/plugins/com/jolla
%endif
%{_handler_datadir}/PresageInputHandler.qml
%{_datadir}/presage/database_empty
%{_sysconfdir}/presage.xml
