Name:           ip-lookup
Version:        1.5.2
Release:        1%{?dist}
Summary:        A simple IP lookup tool
License:        GPL
URL:            https://deadzone.lol
BuildArch:      x86_64

BuildRequires:  gcc, libcurl-devel, cjson-devel
Requires:       curl, cjson

%description
A simple IP lookup tool that shows city, ASN, and other information.

%prep
echo "Prep step; possibly unnecessary for direct binary build"

%build
make -C $(pwd)

%install
install -Dm755 bin/ip-lookup %{buildroot}%{_bindir}/ip-lookup

%files
%{_bindir}/ip-lookup

%changelog
* Mon Feb 26 2024 Rich <rich@bandaholics.cash> - 1.2-1
- Initial package for ip-lookup
