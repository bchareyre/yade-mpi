TEMPLATE=subdirs
SUBDIRS=src
CONFIG += debug \
          warn_on

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
