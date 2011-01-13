###############
Installation
###############

Yade can be installed from packages (precompiled binaries) or source code. The choice depends on what you need: if you don't plan to modify Yade itself, package installation is easier. In the contrary case, you must download and install source code.

Packages
----------

Packages are (as of now) provided for several Ubuntu versions from `Yade package archive <https://launchpad.net/~yade-users/+archive/ppa>`_. Different version of Yade can be installed alongisde each other. The ``yade`` vitual package always depends on the latest stable package, while ``yade-snapshot`` will pull the latest snapshot package. To install quickly, run the following::

	sudo add-apt-repository ppa:yade-users/ppa 
	sudo add-apt-repository ppa:yade-users/external   # optional (updates of other packages)
	sudo apt-get update
	sudo apt-get install yade

More detailed instructions are available at the `archive page <https://launchpad.net/~yade-users/+archive/ppa>`_

Source code
------------

Download
^^^^^^^^^^

If you want to install from source, you can install either a release (numbered version, which is frozen) or the current developement version (updated by the developers frequently). You should download the development version (called ``trunk``) if you want to modify the source code, as you might encounter problems that will be fixed by the developers. Release version will not be modified (except for updates due to critical and easy-to-fix bugs), but they are in a more stabilized state that trunk generally.

#. Releases can be downloaded from the `download page <https://launchpad.net/yade/+download>`_, as compressed archive. Uncompressing the archive gives you a directory with the sources.

#. developement version (trunk) can be obtained from the `code repository <https://code.launchpad.net/yade/>`_ at Launchpad. We use `Bazaar <http://www.bazaar-vcs.org>`_ (the ``bzr`` command) for code management (install the ``bzr`` package in your distribution)::

		bzr checkout lp:yade

   will download the whole code repository of ``trunk``. Check out `Quick Bazaar tutorial <https://yade-dem.org/wiki/Quick_Bazaar_tutorial>`_ wiki page for more. For those behind firewall, `daily snapshot <http://beta.arcig.cz/~eudoxos/yade/yade-trunk-latest.tar.bz2>`_ of the repository (as compressed archive) is provided.

Release and trunk sources are compiled in the same way.

Prerequisities
^^^^^^^^^^^^^^^

Yade relies on a number of external software to run; its installation is checked before the compilation starts. 

* `scons <http://www.scons.org>`_ build system
* `gcc <http://www.gcc.gnu.org>`_ compiler (g++); other compilers will not work; you need g++>=4.2 for openMP support
* `boost <http://www.boost.org/>`_ 1.35 or later
* `qt3 <http://www.qt.nokia.com>`_ library
* `freeglut3 <http://freeglut.sourceforge.net>`_
* `libQGLViewer <http://www.libqglviewer.com>`_
* `python <http://www.python.org>`_, `numpy <http://numpy.scipy.org>`_, `ipython <http://ipython.scipy.org>`_
* `matplotlib <http://matplotlib.sf.net>`_
* `eigen2 <http://eigen.tuxfamily.org>`_ algebra library
* `gdb <http://www.gnu.org/software/gdb>`_ debugger
* `sqlite3 <http://www.sqlite.org>`_ database engine
* `Loki <http://loki-lib.sf.net>`_ library
* `VTK <http://www.vtk.org/>`_ library (optional but recommended)

Most of the list above is very likely already packaged for your distribution. In Ubuntu, it can be all installed by the following command (cut&paste to the terminal)::

	sudo apt-get install scons freeglut3-dev libloki-dev \
	libboost-date-time-dev libboost-filesystem-dev libboost-thread-dev \
	libboost-regex-dev fakeroot dpkg-dev build-essential g++ \
	libboost-iostreams-dev liblog4cxx10-dev python-dev libboost-python-dev ipython \
	python-matplotlib libsqlite3-dev python-numeric python-tk gnuplot doxygen \
	libgts-dev python-pygraphviz libvtk5-dev python-scientific bzr libeigen2-dev \
	binutils-gold python-xlib python-qt4 pyqt4-dev-tools \
	libqglviewer-qt4-dev python-imaging

command line (cut&paste to the terminal under root privileges) for Fedora (not good tested yet!)::

	yum install scons qt3-devel freeglut-devel boost-devel boost-date-time \
	boost-filesystem boost-thread boost-regex fakeroot gcc gcc-c++ boost-iostreams \
	log4cxx log4cxx-devel python-devel boost-python ipython python-matplotlib \
	sqlite-devel python-numeric ScientificPython-tk gnuplot doxygen gts-devel \
	graphviz-python vtk-devel ScientificPython bzr eigen2-devel libQGLViewer-devel \
	loki-lib-devel python-xlib PyQt4 PyQt4-devel python-imaging


Compilation
^^^^^^^^^^^

Inside the directory where you downloaded the sources (ex "yade" if you use bazaar), install Yade to your home directory (without root priviledges)::

	scons PREFIX=/home/username/YADE

If you have a machine that you are the only user on, you can instead change permission on ``/usr/local`` and install subsequently without specifying the ``PREFIX``::

	sudo chown user: /usr/local    # replace "user" with your login name
	scons

There is a number of options for compilation you can change; run ``scons -h`` to see them (see also :ref:`scons-parameters` in the *Programmer's manual*)

The compilation process can take a long time, be patient.

Decreasing RAM usage furing compilation
"""""""""""""""""""""""""""""""""""""""""

Yade demands a large amount of memory for compilation (due to extensive template use). If you have less than 2GB of RAM, it will be, you might encounter difficulties such as the computer being apparently stalled, compilation taking very long time (hours) or erroring out. This command will minimize RAM usage, but the compilation will take longer -- only one file will be compiled simultaneously and files will be "chunked" together one by one::

	scons jobs=1 chunkSize=1
