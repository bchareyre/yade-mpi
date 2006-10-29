isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lLatticeBeamParameters \
        -lLatticeNodeParameters \
	-lLatticeSetParameters \
	-lNonLocalDependency \
        -rdynamic 
QMAKE_LIBDIR = ../../../../bin \
$${YADE_QMAKE_PATH}/lib/yade/yade-package-common/ \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = ../../../../bin 
CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += NonLocalInitializer.hpp 
SOURCES += NonLocalInitializer.cpp 
INCLUDEPATH += ../../../DataClass/PhysicalParameters/LatticeNodeParameters \
../../../DataClass/PhysicalParameters/LatticeBeamParameters \
../../../DataClass/PhysicalParameters/LatticeSetParameters \
../../../DataClass/InteractionPhysics/NonLocalDependency \
$${YADE_QMAKE_PATH}/include

