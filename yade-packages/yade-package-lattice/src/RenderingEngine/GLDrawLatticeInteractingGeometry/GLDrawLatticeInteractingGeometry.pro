isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lLatticeSetGeometry \
	-lLatticeSetParameters \
	-lLatticeBeamParameters \
	-lLatticeNodeParameters \
	-lLatticeInteractingGeometry \
        -lyade-lib-opengl \
	-lyade-lib-computational-geometry \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../DataClass/PhysicalParameters/LatticeSetParameters \
               ../../DataClass/PhysicalParameters/LatticeBeamParameters \
               ../../DataClass/PhysicalParameters/LatticeNodeParameters \
               ../../DataClass/InteractingGeometry/LatticeInteractingGeometry \
               ../../DataClass/GeometricalModel/LatticeSetGeometry 
QMAKE_LIBDIR = ../../../bin \
               $${YADE_QMAKE_PATH}/lib/yade/yade-package-common/ \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = ../../../bin 
CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += GLDrawLatticeInteractingGeometry.hpp 
SOURCES += GLDrawLatticeInteractingGeometry.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
