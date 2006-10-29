isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lSDECLinkGeometry \
        -lElasticContactLaw \
        -lAveragePositionRecorder \
        -lVelocityRecorder \
        -lForceRecorder \
        -lMacroMicroElasticRelationships \
        -lElasticCriterionTimeStepper \
        -lPhysicalActionVectorVector \
        -lInteractionVecSet \
	-lInteractionHashMap \
        -lBodyRedirectionVector \
        -lyade-lib-wm3-math \
        -lInteractingSphere \
        -lInteractingBox \
        -lCundallNonViscousMomentumDamping \
        -lCundallNonViscousForceDamping \
        -lMetaInteractingGeometry \
        -lGravityEngine \
        -lyade-lib-serialization \
        -lPhysicalActionContainerInitializer \
        -lPhysicalActionContainerReseter \
        -lInteractionGeometryMetaEngine \
        -lInteractionPhysicsMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lyade-lib-multimethods \
        -lBox \
        -lSphere \
        -lAABB \
        -lPersistentSAPCollider \
        -lSAPCollider \
        -lMetaInteractingGeometry2AABB \
	-lStiffnessMatrix \
	-lStiffnessCounter \
	-lResultantForceEngine \
	-lTriaxialStressController \
	-lTriaxialCompressionEngine \
	-lStiffnessMatrixTimeStepper \
	-rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/VelocityRecorder \
               ../../Engine/StandAloneEngine/ForceRecorder \
               ../../Engine/StandAloneEngine/AveragePositionRecorder \
               ../../Engine/StandAloneEngine/ElasticCriterionTimeStepper \
               ../../Engine/StandAloneEngine/ElasticContactLaw \
               ../../Engine/EngineUnit/MacroMicroElasticRelationships \
               ../../DataClass/InteractionPhysics/SDECLinkPhysics \
               ../../DataClass/InteractionGeometry/SDECLinkGeometry \
	        ../../DataClass/PhysicalAction/StiffnessMatrix \
	       ../../Engine/StandAloneEngine/StiffnessCounter \	      
	       ../../Engine/DeusExMachina/TriaxialCompressionEngine \
	       ../../Engine/DeusExMachina/TriaxialStressController \
	        ../../Engine/StandAloneEngine/StiffnessMatrixTimeStepper \
               ../../DataClass/PhysicalParameters/BodyMacroParameters 
	       
QMAKE_LIBDIR = ../../../bin \
               ../../../bin \
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
HEADERS += TriaxialTest.hpp 
SOURCES += TriaxialTest.cpp 
