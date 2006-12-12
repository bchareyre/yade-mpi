isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lFrictionLessElasticContactLaw \
        -lRigidBodyParameters \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
        -lPhysicalActionVectorVector \
        -lInteractingSphere \
        -lInteractingBox \
        -lCundallNonViscousForceDamping \
        -lCundallNonViscousMomentumDamping \
        -lMetaInteractingGeometry \
        -lyade-lib-serialization \
        -lyade-lib-base -lWm3Foundation \
        -lPhysicalActionContainerInitializer \
        -lPhysicalActionContainerReseter \
        -lGravityEngine \
        -lInteractionGeometryMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lAABB \
        -lBox \
        -lSphere \
        -lSAPCollider \
        -lMetaInteractingGeometry2AABB \
        -lRotationEngine \
        -lyade-lib-multimethods \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/FrictionLessElasticContactLaw 
QMAKE_LIBDIR = ../../../bin \
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
HEADERS += RotatingBox.hpp 
SOURCES += RotatingBox.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
