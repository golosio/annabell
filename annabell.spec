Name: annabell 
Version: 2.0.9
Release: 	1%{?dist}
Summary: A neural architecture able to learn the natural language
Group: 	Applications/Engineering and Scientific
License: nonstandard
Packager: Bruno Golosio <golosio@uniss.it>
URL:
Source: annabell-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: gcc gcc-c++ libstdc++-devel
Requires: libstdc++

%description
ANNABELL is a large-scale cognitive system, entirely based on an artificial neural network, able to learn complex aspects of natural language starting from tabula rasa, without any a priori knowledge of the structure of phrases, meaning of words, role of the different classes of words, only by communicating with a human through a text-based interface, in a similar way as humans themselves do.

%prep
%setup -q

%build

%configure
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool

make


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
libtool --finish $RPM_BUILD_ROOT%{_libdir}
rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT%{_libdir}/annabell/*.la

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/annabell
%{_prefix}/share/examples/annabell/*
%{_prefix}/share/doc/annabell/annabell_2.0.9_man.pdf


%changelog
* Thu Jun 26 2014 Bruno Golosio
- First version of spec file
