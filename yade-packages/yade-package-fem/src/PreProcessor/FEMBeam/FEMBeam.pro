isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lFEMTetrahedronData \
        -lFEMSetParameters \
        -lFEMLaw \
        -lFEMSetTextLoader \
        -lFEMTetrahedronStiffness \
        -lPhysicalActionVectorVector \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
        -lCundallNonViscousMomentumDamping \
        -lCundallNonViscousForceDamping \
        -lMetaInteractingGeometry \
        -lGravityEngine \
        -lyade-lib-serialization \
        -lyade-lib-wm3-math \
        -lPhysicalActionContainerInitializer \
        -lPhysicalActionContainerReseter \
        -lInteractionGeometryMetaEngine \
        -lInteractionPhysicsMetaEngine \
        -lGeometricalModelMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lAABB \
        -lBox \
        -lRigidBodyParameters \
        -lSphere \
        -lTetrahedron \
        -lTranslationEngine \
        -lyade-lib-multimethods \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/FEMLaw \
               ../../Engine/EngineUnit/FEMTetrahedronStiffness \
               ../../Engine/EngineUnit/FEMSetTextLoader \
               ../../DataClass/PhysicalParameters/FEMTetrahedronData \
               ../../DataClass/PhysicalParameters/FEMSetParameters \
               ../../DataClass/PhysicalParameters/FEMNodeData 
QMAKE_LIBDIR = ../../../bin \
               ../../../bin \
               ../../../bin \
               ../../../bin \
               ../../../bin \
               $${YADE_QMAKE_PATH}/lib/yade/yade-package-common \
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
HEADERS += FEMBeam.hpp 
SOURCES += FEMBeam.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
