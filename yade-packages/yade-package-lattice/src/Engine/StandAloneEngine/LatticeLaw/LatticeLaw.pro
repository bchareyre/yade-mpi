isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lLatticeBeamParameters \
        -lLatticeNodeParameters \
	-lLatticeSetParameters \
	-lLatticeBeamAngularSpring
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../../DataClass/PhysicalParameters/LatticeNodeParameters \
               ../../../DataClass/PhysicalParameters/LatticeSetParameters \
               ../../../DataClass/PhysicalParameters/LatticeBeamParameters \
               ../../../DataClass/InteractionPhysics/LatticeBeamAngularSpring
QMAKE_LIBDIR = ../../../../bin \
               $${YADE_QMAKE_PATH}/lib/yade/yade-package-common/ \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
DESTDIR = ../../../../bin 
CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += LatticeLaw.hpp 
SOURCES += LatticeLaw.cpp 
