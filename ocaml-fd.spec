%define name	ocaml-fd
%define version	1.0.1
%define release	%mkrel 1
%define ocaml_sitelib %(if [ -x /usr/bin/ocamlc ]; then ocamlc -where;fi)/site-lib

Name:		%{name}
Version:	%{version}
Release:	%{release}
Summary:	Descriptor-passing functions for OCaml.
Source: 	http://oss.digirati.com.br/ocaml-fd/ocaml-fd-%{version}.tar.bz2
URL:		http://oss.digirati.com.br/ocaml-fd
License:	LGPL
Group:		Development/Other
BuildRequires:	ocaml
BuildRequires:  findlib
BuildRoot:	%{_tmppath}/%{name}-%{version}

%description
This OCaml library implements miscellaneous functions related to UNIX file
descriptors. Currently, send_fd, recv_fd and fexecve are implemented.

%package	devel
Summary:	Development files for %{name}
Group:		Development/Other
Obsoletes:	%{name}

%description devel
This package contains the development files needed to build applications
using %{name}.

%prep
%setup -q -n ocaml-fd-%{version}

%build
make reallyall
make htdoc

%install
rm -rf %{buildroot}
install -d -m 755 %{buildroot}/%{ocaml_sitelib}
install -d -m 755 %{buildroot}/%{ocaml_sitelib}/stublibs
install -d -m 755 %{buildroot}/%_defaultdocdir/%{name}/html
ocamlfind install fd META -destdir %{buildroot}/%{ocaml_sitelib} \
  fd.cmi fd.mli fd.cma fd.cmxa dllfd_stubs.so libfd_stubs.a fd.a
rm -f %{buildroot}/%{ocaml_sitelib}/stublibs/*.owner

%clean
rm -rf %{buildroot}

%files devel
%defattr(-,root,root)
%doc INSTALL LICENSE doc/fd/html
%{ocaml_sitelib}/fd
%{ocaml_sitelib}/stublibs/dllfd_stubs.so

%changelog
* Thu Aug 09 2007 Andre Nathan <andre@digirati.com.br> 1.0.0-1mdv2008.0
- First release.
