# Plik utworzony przez menad?era projektów kdevelopa
# ------------------------------------------- 
# Podkatalog wzgl?dem g?ównego katalogu projektu: ./libraries/yade-lib-gui-qt
# Cel to biblioteka QtGUI

QtGeneratedSimulationController.ui.target = QtGeneratedSimulationController.ui 
QtGeneratedFrontEnd.ui.target = QtGeneratedFrontEnd.ui 
QtGUISignalCatcher.ui.target = QtGUISignalCatcher.ui 
QtFileGeneratorController.ui.target = QtFileGeneratorController.ui 
LIBS += -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lyade-lib-factory \
        -lyade-lib-opengl \
        -lyade-lib-threads \
        -lXMLManager \
        -lyade-lib-serialization \
        -lyade-lib-serialization-qt \
        -lQGLViewer \
        -lboost_date_time \
        -lboost_filesystem \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../libraries/yade-lib-wm3-math/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-multimethods/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-factory/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-opengl/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-threads/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-serialization-xml/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-serialization/$(YADEDYNLIBPATH) \
               ../../libraries/yade-lib-serialization-qt/$(YADEDYNLIBPATH) \
               ../../yade/Interaction/Interaction/$(YADEDYNLIBPATH) \
               ../../serialization/XMLManager/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/yade-lib-wm3-math/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/yade-lib-multimethods/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/yade-lib-factory/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/yade-lib-opengl/$(YADEDYNLIBPATH) \
               ../../../toolboxes/Libraries/yade-lib-threads/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH) 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
TARGET = QtGUI 
DESTDIR = $(YADEDYNLIBPATH) 
CONFIG += debug \
          warn_on \
          qt \
          dll 
TEMPLATE = lib 
FORMS += YadeQtGeneratedMainWindow.ui \
         QtFileGeneratorController.ui \
         QtGeneratedSimulationController.ui \
         QtGeneratedMessageDialog.ui 
HEADERS += QtGUI.hpp \
           GLViewer.hpp \
           YadeQtMainWindow.hpp \
           QtFileGenerator.hpp \
           SimulationController.hpp \
           QGLThread.hpp \
           SimulationControllerUpdater.hpp \
           MessageDialog.hpp \
           FileDialog.hpp 
SOURCES += QtGUI.cpp \
           GLViewer.cpp \
           YadeQtMainWindow.cpp \
           QtFileGenerator.cpp \
           SimulationController.cpp \
           QGLThread.cpp \
           SimulationControllerUpdater.cpp \
           MessageDialog.cpp \
           FileDialog.cpp 
