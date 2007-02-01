isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lSpheresContactGeometry \
        -lElasticContactInteraction \
	-lSimpleElasticInteraction \
        -lyade-lib-opengl \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../DataClass/InteractionGeometry/SpheresContactGeometry \
	       ../../DataClass/InteractionPhysics/ElasticContactInteraction \
	       ../../../../yade-package-common/src/DataClass/InteractionPhysics/SimpleElasticInteraction \
               ../../RenderingEngine/OpenGLRenderingEngine 
QMAKE_LIBDIR = ../../../bin \
               ../../../../yade-package-common/bin \
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
HEADERS += GLDrawSpheresContactGeometry.hpp 
SOURCES += GLDrawSpheresContactGeometry.cpp 

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<

