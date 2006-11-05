isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


SUBDIRS += AAInteractingBox2InteractingSphere4ClosestFeatures \
           InteractingBox2InteractingBox4ClosestFeatures \
           InteractingBox2InteractingSphere4ClosestFeatures \
           InteractingBox2InteractingSphere4ErrorTolerantContact \
           InteractingSphere2InteractingSphere4ClosestFeatures \
           InteractingSphere2InteractingSphere4ErrorTolerantContact 
CONFIG += debug \
          thread \
warn_on
TEMPLATE = subdirs
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
