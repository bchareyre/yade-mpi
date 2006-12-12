isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lSpheresContactGeometry \
        -lInteractionGeometryMetaEngine \
        -lyade-lib-base -lWm3Foundation \
        -lyade-lib-multimethods \
        -lInteractingSphere \
        -lInteractingBox \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../../DataClass/InteractionGeometry/SpheresContactGeometry 
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
HEADERS += InteractingBox2InteractingSphere4SpheresContactGeometry.hpp 
SOURCES += InteractingBox2InteractingSphere4SpheresContactGeometry.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
