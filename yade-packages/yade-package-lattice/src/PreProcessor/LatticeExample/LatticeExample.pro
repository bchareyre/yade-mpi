isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lLineSegment \
        -lLatticeSetParameters \
        -lLatticeBeamParameters \
        -lLatticeNodeParameters \
	-lNonLocalDependency \
	-lLatticeBeamAngularSpring \
        -lPhysicalActionVectorVector \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
        -lMetaInteractingGeometry \
        -lGeometricalModelMetaEngine \
        -lInteractionPhysicsMetaEngine \
        -lBoundingVolumeMetaEngine \
        -lPhysicalActionApplier \
        -lPhysicalParametersMetaEngine \
        -lPhysicalActionContainerInitializer \
        -lParticleParameters \
        -lAABB \
        -lSphere \
        -lDisplacementEngine \
        -lLatticeLaw \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include/ \
               ../../Engine/StandAloneEngine/LatticeLaw \
               ../../DataClass/PhysicalParameters/LatticeSetParameters \
               ../../DataClass/PhysicalParameters/LatticeNodeParameters \
               ../../DataClass/PhysicalParameters/LatticeBeamParameters \
               ../../DataClass/InteractionPhysics/LatticeBeamAngularSpring \
               ../../DataClass/InteractionPhysics/NonLocalDependency \
               ../../DataClass/GeometricalModel/LineSegment
QMAKE_LIBDIR = ../../../bin \
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
HEADERS += LatticeExample.hpp 
SOURCES += LatticeExample.cpp 
