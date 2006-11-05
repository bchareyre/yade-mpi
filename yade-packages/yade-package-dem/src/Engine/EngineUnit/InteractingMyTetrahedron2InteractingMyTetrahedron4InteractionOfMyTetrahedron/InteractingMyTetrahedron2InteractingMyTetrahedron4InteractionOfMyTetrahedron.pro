isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lInteractionOfMyTetrahedron \
        -lInteractionGeometryMetaEngine \
        -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lInteractingMyTetrahedron \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../../DataClass/InteractionGeometry/InteractionOfMyTetrahedron \
               ../../../DataClass/InteractingGeometry/InteractingMyTetrahedron
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
HEADERS += InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron.hpp 
SOURCES += InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron.cpp 

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
