###############
Installation
###############

Yade can be installed from packages (precompiled binaries) or source code. 
The choice depends on what you need: if you don't plan to modify Yade itself, 
package installation is easier. In the contrary case, you must download and 
install source code.

Packages
----------

Packages from Launchpad PPA service (package personal archive) are 
provided for all currently supported Ubuntu versions for 
`stable <https://launchpad.net/~yade-pkg/+archive/stable>`_ and 
`daily <https://launchpad.net/~yade-pkg/+archive/snapshots>`_ releases.
``yade-daily`` is a automatically daily (if there were some commtis during
the previous days) geenrated package, which includes all the newly added 
features. To install version from PPA, run the following:

* For stable releases::

	sudo add-apt-repository ppa:yade-pkg/stable       # for stable releases 
	sudo add-apt-repository ppa:yade-users/external   # optional (updates of other packages)
	sudo apt-get update
	sudo apt-get install yade-stable

* For latest builds from trunk::

	sudo add-apt-repository ppa:yade-pkg/snapshots    # for latest daily (mostly) releases 
	sudo apt-get update
	sudo apt-get install yade-daily

After you added whether stable or snapshot PPAs, you will get automatically
the updates of the package, when they arrive the PPA.

More detailed instructions are available at the corresponding pages of 
ppa`s (links above).

Since 2011 all Ubuntu versions (starting from 11.10, Oneiric) and Debian (starting from Wheezy) 
are having already Yade in their main repositories. There are only stable releases are placed.
To install the program, run the following::

	sudo apt-get install yade

To check, what version of Yade is in specific Distribution, check the links
for `Ubuntu <https://launchpad.net/ubuntu/+source/yade>`_ and 
`Debian <http://packages.qa.debian.org/y/yade.html>`_.


Source code
------------

Installation from source code is reasonable, when you want to add or 
modify constitutive laws, engines or functions... Installing the latest 
trunk version allows one to use newly added features, which are not yet 
available in packaged versions. 

Download
^^^^^^^^^^

If you want to install from source, you can install either a release 
(numbered version, which is frozen) or the current developement version 
(updated by the developers frequently). You should download the development 
version (called ``trunk``) if you want to modify the source code, as you 
might encounter problems that will be fixed by the developers. Release 
version will not be modified (except for updates due to critical and 
easy-to-fix bugs), but they are in a more stabilized state that trunk 
generally.

#. Releases can be downloaded from the `download page <https://launchpad.net/yade/+download>`_, as compressed archive. Uncompressing the archive gives you a directory with the sources.
#. developement version (trunk) can be obtained from the `code repository <https://github.com/yade/>`_ at github.

We use `GIT <http://git-scm.com/>`_ (the ``git`` command) for code 
management (install the ``git`` package in your distribution and create a GitHub account)::

		git clone git@github.com:yade/trunk.git

will download the whole code repository of ``trunk``. Check out `Yade on github
<https://www.yade-dem.org/wiki/Yade_on_github>`_ wiki page for more.

Alternatively, a read-only checkout is possible via https without a GitHub account (easier if you don't want to modify the main Yade branch)::

		git clone https://github.com/yade/trunk.git
   
For those behind firewall, you can download `any revision  <https://www.yade-dem.org/source/>`_ of the repository as compressed archive.

Release and trunk sources are compiled in the same way.

Prerequisities
^^^^^^^^^^^^^^^

Yade relies on a number of external software to run; its installation is checked before the compilation starts. 

* `cmake <http://www.cmake.org/>`_ build system
* `gcc <http://www.gcc.gnu.org>`_ compiler (g++); other compilers will not work; you need g++>=4.2 for openMP support
* `boost <http://www.boost.org/>`_ 1.35 or later
* `qt4 <http://www.qt.nokia.com>`_ library
* `freeglut3 <http://freeglut.sourceforge.net>`_
* `libQGLViewer <http://www.libqglviewer.com>`_
* `python <http://www.python.org>`_, `numpy <http://numpy.scipy.org>`_, `ipython <http://ipython.scipy.org>`_
* `matplotlib <http://matplotlib.sf.net>`_
* `eigen3 <http://eigen.tuxfamily.org>`_ algebra library
* `gdb <http://www.gnu.org/software/gdb>`_ debugger
* `sqlite3 <http://www.sqlite.org>`_ database engine
* `Loki <http://loki-lib.sf.net>`_ library
* `VTK <http://www.vtk.org/>`_ library (optional but recommended)

Most of the list above is very likely already packaged for your distribution. 
The following commands have to be executed in command line of corresponding 
distributions. Just copy&paste to the terminal. To perform commands you 
should have root privileges

	* **Ubuntu**, **Debian** and their derivatives::

		sudo apt-get install cmake git freeglut3-dev libloki-dev \
		libboost-date-time-dev libboost-filesystem-dev libboost-thread-dev \
		libboost-program-options-dev \
		libboost-regex-dev fakeroot dpkg-dev build-essential g++ \
		libboost-iostreams-dev python-dev libboost-python-dev ipython \
		python-matplotlib libsqlite3-dev python-numpy python-tk gnuplot \
		libgts-dev python-pygraphviz libvtk5-dev python-scientific libeigen3-dev \
		binutils-gold python-xlib python-qt4 pyqt4-dev-tools \
		gtk2-engines-pixbuf python-argparse \
		libqglviewer-qt4-dev python-imaging libjs-jquery python-sphinx python-git python-bibtex \
		libxmu-dev libxi-dev libgmp3-dev libcgal-dev help2man libsuitesparse-dev \
		libopenblas-dev libmetis-dev

	* **Fedora**::

		yum install cmake qt3-devel freeglut-devel boost-devel boost-date-time \
		boost-filesystem boost-thread boost-regex fakeroot gcc gcc-c++ boost-iostreams \
		python-devel boost-python ipython python-matplotlib \
		sqlite-devel python-numeric ScientificPython-tk gnuplot doxygen gts-devel \
		graphviz-python vtk-devel ScientificPython eigen2-devel libQGLViewer-devel \
		loki-lib-devel python-xlib PyQt4 PyQt4-devel python-imaging python-sphinx python-bibtex


Compilation
^^^^^^^^^^^

You should create a separate build-place-folder, where Yade will be configured 
and where the source code will be compiled. Then inside this build-directory you
should start cmake to configure the compilation process::

	cmake -DINSTALL_PREFIX=/path/to/installfolder /path/to/sources

Additional options can be configured in the same line with the following 
syntax::

	cmake -DOPTION1=VALUE1 -DOPTION2=VALUE2
	
The following options are available:
	
	* INSTALL_PREFIX: path where Yade should be installed (/usr/local by default)
	* LIBRARY_OUTPUT_PATH: path to install libraries (lib by default)
	* DEBUG: compile in debug-mode (OFF by default)
	* CMAKE_VERBOSE_MAKEFILE: output additional information during compiling (OFF by default)
	* SUFFIX: suffix, added after binary-names (version number by default)
	* NOSUFFIX: do not add a suffix after binary-name (OFF by default)
	* YADE_VERSION: explicitely set version number (is defined from git-directory by default)
	* ENABLE_GUI: enable GUI option (ON by default)
	* ENABLE_CGAL: enable CGAL option (ON by default)
	* ENABLE_VTK: enable VTK-export option (ON by default)
	* ENABLE_OPENMP: enable OpenMP-parallelizing option (ON by default)
	* ENABLE_GTS: enable GTS-option (ON by default)
	* ENABLE_GL2PS: enable GL2PS-option (ON by default)
	* runtimePREFIX: used for packaging, when install directory is not the same is runtime directory (/usr/local by default)
	* CHUNKSIZE: used, if you want several sources to be compiled at once. Increases compilation speed and RAM-consumption during it (1 by default).

For using an extended parameters of cmake, please, follow the corresponding
documentation on cmake-webpage. 

If the compilation is finished without errors, you will see all enabled 
and disabled options. Then start the standard the compilation process::

	make

Installing performs with the following command::

	make install

The "install" command will in fact also recompile if source files have been modified. 
Hence there is no absolute need to type the two commands separately.

The compilation process can take a long time, be patient. An additional
parameter on many cores systems ``-j`` can be added to decrease compilation time
and split the compilation on many cores. For example, on 4-core machines
it would be reasonable to set the parameter ``-j4``. Note, the Yade requires
approximately 2GB/core for compilation, otherwise the swap-file will be used
and a compilation time dramatically increases.

For building the documentation you should at first execute the command "make install"
and then "make doc" to build it. The generated files will be stored in your current
build directory/doc/sphinx/_build.

"make manpage" command generates and moves manpages in a standard place.
"make check" command executes standard test to check the functionality of compiled program.

Yade can be compiled not only by GCC-compiler, but also by `CLANG <http://clang.llvm.org/>`_ 
front-end for the LLVM compiler. For that you set the environment variables CC and CXX 
upon detecting the C and C++ compiler to use::

	export CC=/usr/bin/clang
	export CXX=/usr/bin/clang++
	cmake -DOPTION1=VALUE1 -DOPTION2=VALUE2

Clang does not support OpenMP-parallelizing for the moment, that is why the 
feature will be disabled.
