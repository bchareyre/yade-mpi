Name:           yade-0.60
Version:        1.0  
Release:        1%{?dist}
Summary:        Platform for discrete element modeling

Group:          Applications/Engineering
License:        GPLv2
URL:            https://launchpad.net/yade
Source0:        http://launchpad.net/yade/trunk/0.60/+download/yade-0.60.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  scons, freeglut-devel, boost-devel >= 1.35, boost-date-time >= 1.35, boost-filesystem >= 1.35, boost-thread >= 1.35, boost-regex >= 1.35, fakeroot, gcc, gcc-c++ > 4.0, boost-iostreams >= 1.35, log4cxx, log4cxx-devel, python-devel, boost-python >= 1.35, ipython, python-matplotlib, sqlite-devel, python-numeric, graphviz-python, vtk-devel, ScientificPython, bzr, eigen2-devel, libQGLViewer-devel, loki-lib-devel, python-xlib, PyQt4, PyQt4-devel, ScientificPython-tk, gnuplot, doxygen, gts-devel
Requires:       ScientificPython, python-numeric, ipython, python-matplotlib, ScientificPython-tk, PyQt4, python-xlib, gnuplot

%description 
Platform for discrete element modeling.
Yet Another Dynamic Engine.

Extensible open-source framework for discrete numerical models, 
focused on Discrete Element Method. 
The computation parts are written in c++ using flexible object model, 
allowing independent implementation of new algorithms and interfaces. 
Python is used for rapid and concise scene construction, 
simulation control, postprocessing and debugging.


%prep
%setup -q


%build


%install
rm -rf $RPM_BUILD_ROOT
scons profile=rpm PREFIX=$RPM_BUILD_ROOT/usr buildPrefix=buildRPM version=%{version} brief=0 chunkSize=5 jobs=1 features=vtk,gts,opengl,openmp,qt4 optimize=1 debug=0 

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/*


%changelog
