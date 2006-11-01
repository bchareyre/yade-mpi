isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/home/janek/YADE, this will not work from inside kdevelop (when they will fix it?)" )
}


HEADERS += BodyContainer.hpp \
           Body.hpp \
           BoundingVolume.hpp \
           DeusExMachina.hpp \
           Engine.hpp \
           FileGenerator.hpp \
           FrontEnd.hpp \
           GeometricalModel.hpp \
           InteractingGeometry.hpp \
           InteractionContainer.hpp \
           InteractionGeometry.hpp \
           Interaction.hpp \
           InteractionPhysics.hpp \
           MetaBody.hpp \
           MetaDispatchingEngine1D.hpp \
           MetaDispatchingEngine2D.hpp \
           MetaEngine.hpp \
           NullGUI.hpp \
           Omega.hpp \
	   SimulationFlow.hpp \
           PhysicalActionContainer.hpp \
           PhysicalAction.hpp \
           PhysicalParameters.hpp \
           Preferences.hpp \
           RenderingEngine.hpp \
           ThreadRunner.hpp \
           ThreadWorker.hpp \
           yadeExceptions.hpp \
           TimeStepper.hpp \
           BodyContainerIterator.hpp \
           BodyContainerIteratorPointer.hpp \
           InteractionContainerIterator.hpp \
           InteractionContainerIteratorPointer.hpp \
           PhysicalActionContainerIterator.hpp \
           PhysicalActionContainerIteratorPointer.hpp \
           MetaDispatchingEngine.hpp \
           EngineUnit.hpp \
           EngineUnit1D.hpp \
           EngineUnit2D.hpp \
	   StandAloneEngine.hpp \
	   BroadInteractor.hpp \
	   InteractionSolver.hpp \
	   DataRecorder.hpp \
           StandAloneSimulator.hpp 
SOURCES += BodyContainer.cpp \
           Body.cpp \
           BoundingVolume.cpp \
           DeusExMachina.cpp \
           FileGenerator.cpp \
           FrontEnd.cpp \
           GeometricalModel.cpp \
           InteractingGeometry.cpp \
           InteractionContainer.cpp \
           Interaction.cpp \
           MetaBody.cpp \
           NullGUI.cpp \
           Omega.cpp \
	   SimulationFlow.cpp \
           PhysicalActionContainer.cpp \
           PhysicalParameters.cpp \
           Preferences.cpp \
           ThreadRunner.cpp \
           ThreadWorker.cpp \
           yade.cpp \
           yadeExceptions.cpp \
           TimeStepper.cpp \
           MetaDispatchingEngine.cpp \
           StandAloneSimulator.cpp 
LIBS += -lyade-lib-serialization \
-lyade-lib-factory \
-lyade-lib-wm3-math \
-lyade-lib-loki \
-lyade-lib-multimethods \
-lglut \
-rdynamic \
-lboost_date_time \
-lboost_filesystem \
-lboost_thread
INCLUDEPATH += $${YADE_QMAKE_PATH}/include
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs
win32 {
TARGET = ../../../bin/yade
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade
}

CONFIG += debug \
          thread \
warn_on
TEMPLATE = app
