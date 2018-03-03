# Template for generation of keyboard RPMs
# for Presage on Sailfish. This temlate is used
# by package-keyboard.sh script

# Prevent brp-python-bytecompile from running.
%define __os_install_post %{___build_post}

# "Harbour RPM packages should not provide anything."
%define __provides_exclude_from ^%{_datadir}/.*$

Name: keyboard-presage-__langcode__
Version: __version__
Release: 1
Summary: Keyboard layout for __Language__ with Presage support
License: MIT
URL: https://github.com/martonmiklos/sailfishos-presage-predictor
Source: %{name}-%{version}.tar.xz
BuildArch: noarch
Requires: presage-lang-__langcode__
Requires: hunspell-lang-__langcode__
Requires: maliit-plugin-presage

%description
Keyboard layout for __Language__ language with Presage text predictions

%prep
%setup -q

%install
mkdir -p %{buildroot}/usr/share/maliit/plugins/com/jolla/layouts
cp -r keyboard/* %{buildroot}/usr/share/maliit/plugins/com/jolla/layouts

%files
%defattr(-,root,root,-)
%{_datadir}/maliit/plugins/com/jolla/layouts
