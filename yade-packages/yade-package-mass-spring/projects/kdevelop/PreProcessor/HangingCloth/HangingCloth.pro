isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lSpringGeometry \
        -lSpringPhysics \
        -lParticleSetParameters \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
        -lPhysicalActionVectorVector \
        -lInteractingSphere \
        -lInteractingBox \
        -lSDECLinkPhysics \
        -lCundallNonViscousMomentumDamping \
        -lCundallNonViscousForceDamping \
        -lMacroMicroElasticRelationships \
        -lyade-lib-serialization \
        -lyade-lib-wm3-math \
        -lPhysicalActionContainerInitializer \
        -lPhysicalActionContainerReseter \
        -lGravityEngine \
        -lInteractionGeometryMetaEngine \
        -lInteractionPhysicsMetaEngine \
        -lGeometricalModelMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lMesh2D \
        -lAABB \
        -lSphere \
        -lClosestFeatures \
        -lPersistentSAPCollider \
        -lBox \
        -lSDECLinkGeometry \
        -lSAPCollider \
        -lRigidBodyParameters \
        -lBodyMacroParameters \
        -lMetaInteractingGeometry2AABB \
        -lyade-lib-multimethods \
        -lMassSpringLaw \
        -lElasticContactLaw \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/MassSpringLaw \
               ../../Engine/EngineUnit/ParticleSet2Mesh2D \
               ../../DataClass/InteractionPhysics/SpringPhysics \
               ../../DataClass/InteractionGeometry/SpringGeometry \
               ../../DataClass/PhysicalParameters/ParticleSetParameters 
QMAKE_LIBDIR = ../../../../bin \
               ../../../../bin \
               ../../../../bin \
               $${YADE_QMAKE_PATH}/lib/yade/yade-package-dem \
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
HEADERS += HangingCloth.hpp 
SOURCES += HangingCloth.cpp 
