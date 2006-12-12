isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lElasticContactInteraction \
	-lSimpleElasticInteraction \
	-lInteractionOfMyTetrahedron \
        -lElasticBodyParameters \
        -lyade-lib-serialization \
        -lyade-lib-base -lWm3Foundation \
        -lyade-lib-multimethods \
        -lForce \
        -lMomentum \
        -lSphere \
        -lRigidBodyParameters \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../../DataClass/InteractionPhysics/ElasticContactInteraction \
               ../../../DataClass/InteractionGeometry/InteractionOfMyTetrahedron \
               ../../../DataClass/PhysicalParameters/ElasticBodyParameters 
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
HEADERS += MyTetrahedronLaw.hpp 
SOURCES += MyTetrahedronLaw.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
