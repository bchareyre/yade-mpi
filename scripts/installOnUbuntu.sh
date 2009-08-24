#!/bin/bash
# This script works on Ubuntu 9.04, and possibly on new Debians.
# It "compiles and installs" the latest YADE BZR-version.
# The install is maximal: you will get almost all YADE functionality, even if you dont need it
# Please, copy this script to the empty directory and start it.
# 


BASEDIR='.'
VERSION='trunk'
sudo aptitude install scons libqt3-mt-dev qt3-dev-tools freeglut3-dev libboost-date-time-dev libboost-filesystem-dev libboost-thread-dev libboost-regex-dev fakeroot dpkg-dev build-essential g++ libboost-iostreams-dev liblog4cxx10-dev python-dev libboost-python-dev ipython python-matplotlib libsqlite3-dev python-numeric python-tk gnuplot doxygen python-pygraphviz python-epydoc
bzr checkout lp:yade
mkdir $BASEDIR/build
mkdir $BASEDIR/build/data
cd $BASEDIR/yade
scons PREFIX=$BASEDIR/../build optimize=1 -j2 features=log4cxx,python,opengl,gts,openmp version=$VERSION linkStrategy=monolithic
scons doc
cd doc
$BASEDIR/../../build/bin/yade-$VERSION yade-epydoc.py

