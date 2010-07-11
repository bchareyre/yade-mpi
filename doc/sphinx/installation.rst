###############
Installation
###############

Installation Yade process is relatively simple: `scons <http://www.scons.org/>`_ takes care about it during build process from source code, package manager resolves dependencies while installing a package.

If you met a problem during installation process, please, `let us know <https://launchpad.net/yade>`_.

Download
===============

First of all you should decide, how to install Yade: from **packages** or from **source code**.

* **packages** allow quickly install the software and use it. But if you need to change something in Yade (add new constitutive laws, new engines etc.) you definitely have to compile Yade from source code

* **source code** compiling requires some more time and effort. After that you will be able to change it according to your needs.

Sorce code
---------------

Release (the latest 0.50) and source code from the trunk are both available.

Release
^^^^^^^^^^^^^^^

Grab the latest release from Launchpad `yade-0.50 <http://launchpad.net/yade/trunk/0.5/+download/yade-0.50.tar.gz>`_ (1.7MB).

Latest BZR snapshot
^^^^^^^^^^^^^^^
You can try the latest yade version from Launchpad. We try to keep it always workable. It can be downloaded, using one of the ways shown below:

* From Launchpad BZR server: ::

	bzr checkout lp:yade

* Daily repository snapshot, `tarball <http://beta.arcig.cz/~eudoxos/yade/yade-trunk-latest.tar.bz2>`_

Also check out a `Quick Bazaar tutorial <https://yade-dem.org/wiki/Quick_Bazaar_tutorial>`_. 

Packages
---------------

For the moment, we have packages for Ubuntu (+derivatives) latest releases.

Ubuntu(+derivatives)
^^^^^^^^^^^^^^^

First of all you must add Yade ppa:

* Adding the PPA to Ubuntu 9.10 (Karmic) and later: ::

	sudo add-apt-repository ppa:yade-users/ppa

* For older Ubuntu versions, please, visit `Yade package page <https://launchpad.net/~yade-users/+archive/ppa>`_ and follow instructions from *Adding this PPA to your system* section.  

Prerequistes
===============

All libraries listed below must be installed with development headers. It's better to install them using your linux's distribution package management system, instead of downloading directly from the website.

Required packages
---------------

* `boost library <http://www.boost.org/>`_ 1.34 or later with components: date-time, filesystem, thread, regex, algorithm, foreach
* qt3 library, including multithreading (-mt) component
* freeglut3 / libglut3 development files
* make
* g++ (other compilers will most likely not work; you need g++>=4.2 for openMP support)
* sqlite3
* scons build system
* python-dev, python-scientific, python-numeric
* `eigen  <http://eigen.tuxfamily.org>`_ mathematic library
* `vtk-headers <http://www.vtk.org/>`_

Installing needed packages for Ubuntu(+derivatives)
---------------

Next command will allow you to install all necessary packages for Ubuntu. Just copy/paste it to your terminal: ::

	sudo apt-get install scons libqt3-mt-dev qt3-dev-tools freeglut3-dev \
	libboost-date-time-dev libboost-filesystem-dev libboost-thread-dev \
	libboost-regex-dev fakeroot dpkg-dev build-essential g++ \
	libboost-iostreams-dev liblog4cxx10-dev python-dev libboost-python-dev ipython \
	python-matplotlib libsqlite3-dev python-numeric python-tk gnuplot doxygen \
	libgts-dev python-pygraphviz libvtk5-dev python-scientific bzr libeigen2-dev \
	libqglviewer-qt3-dev

Source code compiling
===============

Being inside the directory where you downloaded the sources (ex "yade" if you use bazaar), run this command to install yade as a local user without root privileages: ::

	scons PREFIX=/home/username/YADE

If you have a machine that you are the only user on, you can instead change permission on /usr/local ::

	sudo chown username: /usr/local

If you want to change additional parameters, you can call: ::

	scons -h

It will give a list of possible parameters. Compilation process can take a long time.

How to decrease Yade RAM demand during compilation
---------------

Last Yade version are demanding a large amount of memory for compilation. If you have less, than 2GB available RAM, it will be, probably difficult to compile a Yade.
If so, next command will decrease Yade demanding of memory to minimal, but compilation will take longer: ::

	scons jobs=1 chunkSize=1

Upper command dictates Yade to use just 1 core (if your CPU has more, than 1 core, all of them will be used for compiling by default); the second parameter decreases the number of compiled engines to 1 in 1 "portion". See *scons -h* for additional information.
