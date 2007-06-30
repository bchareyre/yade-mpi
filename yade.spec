#%define defREVISIONS if [ -f RELEASE ]; then REVISION=`cat RELEASE` SNAPSHOT="" elif [ -d .svn ]; then REVISION="svn$( svn info | egrep '^Revision:' | cut -f2 -d' ')" SNAPSHOT="-snapshot" elif [ -d .bzr ]; then REVISION="bzr$( bzr version-info | egrep '^revno:' | cut -f2 -d' ')" SNAPSHOT="-snapshot" else echo 'Revision could not be determined (RELEASE or .svn or .bzr)' >2 exit 1 fi; if [ -z "$REVISION" ]; then _REVISION=""; else _REVISION=_$REVISION; export REVISION SNAPSHOT _REVISION

%define REVISION 0.11
%define _REVISION -0.11
%define SNAPSHOT -stable

# do not strip files after compilation
%define __spec_install_post /usr/lib/rpm/brp-compress

#%defREVISIONS

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

%package dbg
Summary: Platform for discrete numerical modeling, debugging version.
Group: Applications/Engineering

%package dev
Summary: Platform for discrete numerical modeling, development files.
Group: Applications/Engineering

%description
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling.

%description dbg
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling. This packages is built with debugging support.

%description dev
YADE is an Open Source GNU/GPL Software framework designed with dynamic libraries in a way that it is easy to add new numerical models for simulation. It aims to provide versatile means to perform discrete modelling. This package contains development files.


%define RPM_BUILD_ROOT /var/tmp/%{name}-%{version}-buildroot

%prep
%setup -n yade-0.11.0

%build

%install
set
#optimized build (main package)
scons buildPrefix=${RPM_BUILD_DIR} PREFIX=%{RPM_BUILD_ROOT}/usr runtimePREFIX=/usr version=%{REVISION} useMiniWm3=1 exclude=realtime-rigidbody variant='' optimize=1 debug=0
#debug build (-dbg)
scons variant=-dbg optimize=0 debug=1
# install headers (-dev)
scons %{RPM_BUILD_ROOT}/usr/include %{RPM_BUILD_ROOT}/usr/lib/pkgconfig

%files
/usr/lib/yade{$_REVISION}
/usr/bin/yade{$_REVISION}

%files dbg
/usr/lib/yade{$_REVISION}-dbg
/usr/bin/yade{$_REVISION}-dbg

%files dev
/usr/include/yade{$_REVISION}
/usr/lib/pkgconfig/yade{$_REVISION}*.pc

