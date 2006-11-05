isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lLineSegment \
        -lLatticeSetParameters \
        -lLatticeBeamParameters \
        -lLatticeNodeParameters \
        -lNonLocalDependency \
        -lNonLocalInitializer \
        -lStrainRecorder \
        -lMeasurePoisson \
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
               ../../Engine/StandAloneEngine/StrainRecorder \
               ../../Engine/StandAloneEngine/MeasurePoisson \
               ../../Engine/StandAloneEngine/NonLocalInitializer \
               ../../DataClass/PhysicalParameters/LatticeSetParameters \
               ../../DataClass/PhysicalParameters/LatticeNodeParameters \
               ../../DataClass/PhysicalParameters/LatticeBeamParameters \
               ../../DataClass/InteractionPhysics/LatticeBeamAngularSpring \
               ../../DataClass/InteractionPhysics/NonLocalDependency \
               ../../DataClass/GeometricalModel/LineSegment
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
HEADERS += LatticeExample.hpp 
SOURCES += LatticeExample.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
