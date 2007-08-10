%define name	ocaml-fd
%define version	1.0.0
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
This OCaml library implements descriptor-passing functions. Tipically, this
functionality is used to allow a process to perform all the actions needed
to acquire a descriptor, which is then sent to another process, which will
then handle the data transfer operations on that descriptor.

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
make all
make allopt
make doc

%install
rm -rf %{buildroot}
install -d -m 755 %{buildroot}/%{ocaml_sitelib}
install -d -m 755 %{buildroot}/%{ocaml_sitelib}/stublibs
install -d -m 755 %{buildroot}/%_defaultdocdir/%{name}/html
ocamlfind install fd META -destdir %{buildroot}/%{ocaml_sitelib} \
  fd.cmi fd.mli fd.cma fd.cmxa dllfd.so libfd.a fd.a
install -m 644 doc/* %{buildroot}/%_defaultdocdir/%{name}/html
rm -f %{buildroot}/%{ocaml_sitelib}/stublibs/*.owner

%clean
rm -rf %{buildroot}

%files devel
%defattr(-,root,root)
%doc INSTALL LICENSE
%{ocaml_sitelib}/fd
%{ocaml_sitelib}/stublibs/dllfd.so
%_defaultdocdir/%{name}/html

%changelog
* Thu Aug 09 2007 Andre Nathan <andre@digirati.com.br> 1.0.0-1mdv2008.0
- First release.