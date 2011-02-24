Name:           yade-0.60
Version:        0.60  
Release:        1%{?dist}
Summary:        Platform for discrete element modeling

Group:          Applications/Engineering
License:        GPLv2
URL:            https://launchpad.net/yade
Source0:        http://launchpad.net/yade/trunk/0.60/+download/yade-0.60.1.tar.bz2
patch0:         exclude-rpath-patch-060
patch1:         disable-info.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}

BuildRequires:  scons, freeglut-devel, boost-devel >= 1.35, boost-date-time >= 1.35, boost-filesystem >= 1.35, boost-thread >= 1.35, boost-regex >= 1.35, fakeroot, gcc, gcc-c++ > 4.0, boost-iostreams >= 1.35, log4cxx, log4cxx-devel, python-devel, boost-python >= 1.35, ipython, python-matplotlib, sqlite-devel, python-numeric, graphviz-python, vtk-devel, ScientificPython, bzr, eigen2-devel, libQGLViewer-devel, loki-lib-devel, python-xlib, PyQt4, PyQt4-devel, ScientificPython-tk, gnuplot, doxygen, gts-devel, texlive-xetex, texlive-latex
Requires:       ScientificPython, python-numeric, ipython, ScientificPython-tk, PyQt4, gnuplot
Requires(post): info
Requires(preun): info

%description 
Platform for discrete element modeling.
Yet Another Dynamic Engine.

Extensible open-source framework for discrete numerical models, 
focused on Discrete Element Method. 
The computation parts are written in c++ using flexible object model, 
allowing independent implementation of new algorithms and interfaces. 
Python is used for rapid and concise scene construction, 
simulation control, postprocessing and debugging.

%package dbg
Summary: Debug Yade version
%description dbg
This package contains debug-libraries for yade

%package doc
Summary: Documentation for Yade
%description doc
This package contains examples, test scripts and documentation.

%prep
%setup -q
%patch1
 
%build

%install
rm -f scons.profile-rpm
rm -rf %{buildroot}/*

#Compile and install optimized version 
scons profile=rpm PREFIX=%{buildroot}/usr runtimePREFIX=/usr buildPrefix=rpm march= brief=0 chunkSize=5 jobs=1 version=0.60 features=vtk,gts,opengl,openmp,qt4 optimize=1 debug=0 variant='' 

#Compile and install debug version 
scons profile=rpm brief=0 chunkSize=5 jobs=1 optimize=0 debug=1 variant='' %{buildroot}%_libdir

#Copy a library "by hand" (not sure, why the symling is not working in RPMs)
rm %{buildroot}%_libdir/%{name}/lib/lib_gts__python-module.so
cp %{buildroot}%_libdir/%{name}/py/gts/_gts.so %{buildroot}%_libdir/%{name}/lib/lib_gts__python-module.so

#Generate documentation
cd %_builddir/%{name}-%{version}/doc/sphinx
PYTHONPATH=. YADE_PREFIX=%{buildroot}/usr %{buildroot}/usr/bin/%{name} yadeSphinx.py
cd _build/latex; xelatex Yade.tex; xelatex Yade.tex; xelatex Yade.tex;

YADE_PREFIX=%{buildroot}/usr %{buildroot}/usr/bin/%{name} --generate-manpage yade-%{version}.1
YADE_PREFIX=%{buildroot}/usr %{buildroot}/usr/bin/%{name}-batch --generate-manpage yade-%{version}-batch.1
cd ../..;

mkdir -p %{buildroot}%{_docdir}/%{name}
mkdir -p %{buildroot}%{_mandir}/man1
mv %_builddir/%{name}-%{version}/doc/sphinx/_build/html %{buildroot}%{_docdir}/%{name}
mv %_builddir/%{name}-%{version}/doc/sphinx/_build/latex/Yade.pdf %{buildroot}%{_docdir}/%{name}
mv %_builddir/%{name}-%{version}/examples %{buildroot}%{_docdir}/%{name}
mv %_builddir/%{name}-%{version}/doc/sphinx/_build/latex/yade-%{version}.1 %{buildroot}%{_mandir}/man1/
mv %_builddir/%{name}-%{version}/doc/sphinx/_build/latex/yade-%{version}-batch.1 %{buildroot}%{_mandir}/man1/

%clean
rm -rf %{buildroot}
rm -rf %{_builddir}/*



%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/%{name}/lib/*
%{_libdir}/%{name}/plugins/*
%{_libdir}/%{name}/py/*
%doc %{_mandir}/*

%files dbg
%defattr(-,root,root,-)
%{_libdir}/%{name}/dbg/*


%files doc
%defattr(-,root,root,-)
%doc %{_docdir}/%{name}/*

%check
YADE_PREFIX=%{buildroot}/usr %{buildroot}/usr/bin/%{name} --test
YADE_PREFIX=%{buildroot}/usr %{buildroot}/usr/bin/%{name} --debug --test

%changelog
* Wed Feb 23 2011 Anton Gladky <gladky.anton@gmail.com> 0.60-1
- Initial packaging
