#
# This needs to be tuned by hand
#
%define REVISION 0.11
%define _REVISION -0.11
%define SNAPSHOT -stable

# do not strip files after compilation
%define __spec_install_post /usr/lib/rpm/brp-compress

# do not generate debug package
%define debug_package %{nil}

Name: yade%{_REVISION}
Provides: yade%{SNAPSHOT}
Version: 1
Release: a
Summary: Platform for discrete numerical modeling
License: GPL
Requires: boost python libQGLViewer
BuildRequires: qt-devel boost-devel scons libXmu libXmu-devel glut-devel ScientificPython python-devel
Url: http://yade.wikia.com
Source: http://download.berlios.de/yade/yade-0.11.0.tar.gz
Group: Applications/Engineering
BuildRoot: %{_tmppath}/%{name}-buildroot

%package dbg
Summary: Platform for discrete numerical modeling, debugging version.
Group: Applications/Engineering
Provides: yade%{SNAPSHOT}-dbg

%package devel
Summary: Platform for discrete numerical modeling, development files.
Group: Applications/Engineering
Provides: yade%{SNAPSHOT}-devel
Requires: boost-devel python-devel qt-devel libQGLViewer-devel glut-devel

%description
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling.

%description dbg
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling. This packages is built with debugging support.

%description devel
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling. This package contains development files.

%prep
%setup -n yade-0.11.0

%build

%install
#optimized build (main package)
scons buildPrefix=${RPM_BUILD_DIR} PREFIX=%{RPM_BUILD_ROOT}/usr runtimePREFIX=/usr version=%{REVISION} useMiniWm3=1 exclude=realtime-rigidbody variant='' optimize=1 debug=0
#debug build (-dbg)
scons variant=-dbg optimize=0 debug=1
# install headers (-devel)
scons %{RPM_BUILD_ROOT}/usr/include %{RPM_BUILD_ROOT}/usr/lib/pkgconfig


%files
%defattr(-,root,root)
/usr/lib/%name
/usr/bin/%name

%files dbg
%defattr(-,root,root)
/usr/lib/%name-dbg
/usr/bin/%name-dbg

%files devel
%defattr(-,root,root)
/usr/include/%name
/usr/lib/pkgconfig/%{name}*.pc

