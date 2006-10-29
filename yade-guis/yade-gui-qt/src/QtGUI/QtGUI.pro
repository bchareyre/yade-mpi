isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


YadeQtGeneratedMainWindow.ui.target = YadeQtGeneratedMainWindow.ui 
YadeQtGeneratedMainWindow.ui.commands = $$IDL_COMPILER 
QtGeneratedSimulationPlayerController.ui.target = QtGeneratedSimulationPlayerController.ui 
QtGeneratedSimulationPlayerController.ui.commands = $$IDL_COMPILER 
QtGeneratedSimulationPlayer.ui.target = QtGeneratedSimulationPlayer.ui 
QtGeneratedSimulationPlayer.ui.commands = $$IDL_COMPILER 
QtGeneratedSimulationController.ui.target = QtGeneratedSimulationController.ui 
QtGeneratedSimulationController.ui.commands = $$IDL_COMPILER 
QtGeneratedPreferencesEditor.ui.target = QtGeneratedPreferencesEditor.ui 
QtGeneratedPreferencesEditor.ui.commands = $$IDL_COMPILER 
QtGeneratedMessageDialog.ui.target = QtGeneratedMessageDialog.ui 
QtGeneratedMessageDialog.ui.commands = $$IDL_COMPILER 
QtGeneratedEngineEditor.ui.target = QtGeneratedEngineEditor.ui 
QtGeneratedEngineEditor.ui.commands = $$IDL_COMPILER 
QtGeneratedCodeGenerator.ui.target = QtGeneratedCodeGenerator.ui 
QtGeneratedCodeGenerator.ui.commands = $$IDL_COMPILER 
QtFileGeneratorController.ui.target = QtFileGeneratorController.ui 
QtFileGeneratorController.ui.commands = $$IDL_COMPILER 
QMAKE_gDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
LIBS += -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lyade-lib-factory \
        -lyade-lib-opengl \
        -lyade-lib-serialization \
        -lyade-lib-computational-geometry \
        -lXMLFormatManager \
        -lyade-lib-serialization-qt \
        -lQGLViewer \
        -lboost_date_time \
        -lboost_filesystem \
        -rdynamic 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs 
win32 {
TARGET = ../../../bin/QtGUI 
CONFIG += console
}
!win32 {
TARGET = ../../bin/QtGUI 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
FORMS += QtFileGeneratorController.ui \
         QtGeneratedCodeGenerator.ui \
         QtGeneratedEngineEditor.ui \
         QtGeneratedMessageDialog.ui \
         QtGeneratedPreferencesEditor.ui \
         QtGeneratedSimulationController.ui \
         YadeQtGeneratedMainWindow.ui \
         QtGeneratedMetaDispatchingEngineProperties.ui \
         QtGeneratedSphericalDEMSimulator.ui \
         QtGeneratedSimulationPlayer.ui 
IDLS += QtGeneratedSimulationPlayerController.ui \
        QtGeneratedSimulationPlayer.ui 
HEADERS += FileDialog.hpp \
           GLEngineEditor.hpp \
           GLViewer.hpp \
           MessageDialog.hpp \
           QtCodeGenerator.hpp \
           QtEngineEditor.hpp \
           QtFileGenerator.hpp \
           QtGUI.hpp \
           QtPreferencesEditor.hpp \
           SimulationController.hpp \
           SimulationControllerUpdater.hpp \
           YadeQtMainWindow.hpp \
           QtGUIPreferences.hpp \
           QtMetaDispatchingEngineProperties.hpp \
           QtSphericalDEM.hpp \
           QtSimulationPlayer.hpp \
           GLSimulationPlayerViewer.hpp 
SOURCES += FileDialog.cpp \
           GLEngineEditor.cpp \
           GLViewer.cpp \
           MessageDialog.cpp \
           QtCodeGenerator.cpp \
           QtEngineEditor.cpp \
           QtFileGenerator.cpp \
           QtGUI.cpp \
           QtPreferencesEditor.cpp \
           SimulationController.cpp \
           SimulationControllerUpdater.cpp \
           YadeQtMainWindow.cpp \
           QtGUIPreferences.cpp \
           QtMetaDispatchingEngineProperties.cpp \
           QtSphericalDEM.cpp \
           QtSimulationPlayer.cpp \
           GLSimulationPlayerViewer.cpp 
