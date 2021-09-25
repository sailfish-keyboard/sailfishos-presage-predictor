Name: keyboard-presage
Version: 1.1
Release: 1
Summary: Keyboard layouts with Presage support
License: MIT
URL: https://github.com/sailfish-keyboard/sailfishos-presage-predictor
Source: %{name}-%{version}.tar.xz
BuildArch: noarch

%description
Keyboard layouts with Presage text predictions. Empty package, do not install

###

%package de_DE
Summary: Keyboard layout for German with Presage support
Requires: presage-lang-de_DE
Requires: hunspell-lang-de_DE
Requires: maliit-plugin-presage

%description de_DE
%{summary}.


%package en_US
Summary: Keyboard layout for English with Presage support
Requires: presage-lang-en_US
Requires: hunspell-lang-en_US
Requires: maliit-plugin-presage

%description en_US
%{summary}.


%package es_ES
Summary: Keyboard layout for Spanish with Presage support
Requires: presage-lang-es_ES
Requires: hunspell-lang-es_ES
Requires: maliit-plugin-presage

%description es_ES
%{summary}.


%package et_EE
Summary: Keyboard layout for Estonian with Presage support
Requires: presage-lang-et_EE
Requires: hunspell-lang-et_EE
Requires: maliit-plugin-presage

%description et_EE
%{summary}.


%package fi_FI
Summary: Keyboard layout for Finnish with Presage support
Requires: presage-lang-fi_FI
Requires: hunspell-lang-fi_FI
Requires: maliit-plugin-presage

%description fi_FI
%{summary}.


%package hu_HU
Summary: Keyboard layout for Hungarian with Presage support
Requires: presage-lang-hu_HU
Requires: hunspell-lang-hu_HU
Requires: maliit-plugin-presage

%description hu_HU
%{summary}.


%package ru_RU
Summary: Keyboard layout for Russian with Presage support
Requires: presage-lang-ru_RU
Requires: hunspell-lang-ru_RU
Requires: maliit-plugin-presage

%description ru_RU
%{summary}.


%package sv_SE
Summary: Keyboard layout for Russian with Presage support
Requires: presage-lang-sv_SE
Requires: hunspell-lang-sv_SE
Requires: maliit-plugin-presage

%description sv_SE
%{summary}.


###

%prep
%setup -q

%install
mkdir -p %{buildroot}/usr/share/maliit/plugins/com/jolla/layouts
cp -r keyboard/* %{buildroot}/usr/share/maliit/plugins/com/jolla/layouts

%files
%defattr(-,root,root,-)

%files de_DE
%{_datadir}/maliit/plugins/com/jolla/layouts/de-presage.*

%files en_US
%{_datadir}/maliit/plugins/com/jolla/layouts/en-presage.*

%files es_ES
%{_datadir}/maliit/plugins/com/jolla/layouts/es-presage.*

%files et_EE
%{_datadir}/maliit/plugins/com/jolla/layouts/et-presage.*

%files fi_FI
%{_datadir}/maliit/plugins/com/jolla/layouts/fi*-presage.*

%files hu_HU
%{_datadir}/maliit/plugins/com/jolla/layouts/hu-presage.*

%files ru_RU
%{_datadir}/maliit/plugins/com/jolla/layouts/ru-presage.*

%files sv_SE
%{_datadir}/maliit/plugins/com/jolla/layouts/sv*-presage.*
