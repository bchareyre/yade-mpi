isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lMyTetrahedronLaw \
        -lMacroMicroElasticRelationships \
        -lElasticCriterionTimeStepper \
        -lPhysicalActionVectorVector \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
	-lInteractingMyTetrahedron \
        -lInteractingSphere \
        -lInteractingBox \
        -lCundallNonViscousMomentumDamping \
        -lCundallNonViscousForceDamping \
        -lMetaInteractingGeometry \
        -lGravityEngine \
        -lPhysicalActionContainerInitializer \
        -lPhysicalActionContainerReseter \
        -lInteractionGeometryMetaEngine \
	-lInteractingGeometryMetaEngine \
        -lInteractionPhysicsMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lBox \
        -lSphere \
        -lAABB \
        -lPersistentSAPCollider \
        -lParticleParameters \
        -lSAPCollider \
        -lMetaInteractingGeometry2AABB \
        -lTetrahedron \
        -lyade-lib-serialization \
        -lyade-lib-base -lWm3Foundation \
        -lyade-lib-multimethods \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/ElasticCriterionTimeStepper \
               ../../Engine/StandAloneEngine/MyTetrahedronLaw \
               ../../DataClass/InteractingGeometry/InteractingMyTetrahedron
QMAKE_LIBDIR = ../../../bin \
               ../../../bin \
               ../../../bin \
               ../../../bin \
               ../../../bin \
               ../../../bin \
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
HEADERS += TetrahedronsTest.hpp 
SOURCES += TetrahedronsTest.cpp 

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<

