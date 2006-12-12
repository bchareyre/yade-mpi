isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lInteractionGeometryMetaEngine \
        -lyade-lib-base -lWm3Foundation \
        -lyade-lib-multimethods \
        -lInteractingSphere \
        -lInteractingBox \
        -lBox \
        -lSphere \
        -lClosestFeatures \
        -rdynamic 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-package-common/ \
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
HEADERS += AAInteractingBox2InteractingSphere4ClosestFeatures.hpp 
SOURCES += AAInteractingBox2InteractingSphere4ClosestFeatures.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
