isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lInteractionGeometryMetaEngine \
        -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lInteractingBox \
        -lInteractingSphere \
        -lBox \
        -lSphere \
        -lClosestFeatures \
        -rdynamic 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-package-common/ \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs 
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
HEADERS += Box2Sphere4ClosestFeatures.hpp 
SOURCES += Box2Sphere4ClosestFeatures.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

