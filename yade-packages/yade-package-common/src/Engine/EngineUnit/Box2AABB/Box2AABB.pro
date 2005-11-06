isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lBoundingVolumeMetaEngine \
        -lInteractingBox \
        -lAABB \
        -lBox \
        -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../../DataClass/InteractingGeometry/InteractingBox \
               ../../../DataClass/BoundingVolume/AABB \
               ../../../Engine/MetaEngine/BoundingVolumeMetaEngine 
QMAKE_LIBDIR = ../../../../../bin \
               ../../../../../bin \
               ../../../../../bin \
               ../../../../../bin \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = ../../../../../bin 
CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += Box2AABB.hpp 
SOURCES += Box2AABB.cpp 
