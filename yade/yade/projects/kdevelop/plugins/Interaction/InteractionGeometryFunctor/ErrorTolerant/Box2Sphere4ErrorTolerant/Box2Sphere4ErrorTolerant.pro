# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/Interaction/InteractionGeometryFunctor/ErrorTolerant/Box2Sphere4ErrorTolerant
# Target is a library:  

LIBS += -lBody \
        -lInteraction \
        -lMultiMethods \
        -lFactory \
        -lMath \
        -lErrorTolerantContactModel \
        -lSerialization \
        -lBox \
        -lSphere \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../../yade/Body/Body/$(YADEDYNLIBPATH) \
               ../../../../../yade/Interaction/Interaction/$(YADEDYNLIBPATH) \
               ../../../../../toolboxes/Libraries/MultiMethods/$(YADEDYNLIBPATH) \
               ../../../../../toolboxes/Libraries/Factory/$(YADEDYNLIBPATH) \
               ../../../../../toolboxes/Libraries/Math/$(YADEDYNLIBPATH) \
               ../../../../../plugins/Interaction/InteractionGeometry/ErrorTolerantContactModel/$(YADEDYNLIBPATH) \
               ../../../../../toolboxes/Libraries/Serialization/$(YADEDYNLIBPATH) \
               ../../../../../plugins/Body/GeometricalModel/Box/$(YADEDYNLIBPATH) \
               ../../../../../plugins/Body/GeometricalModel/Sphere/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH) 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = $(YADEDYNLIBPATH) 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += Box2Sphere4ErrorTolerant.hpp 
SOURCES += Box2Sphere4ErrorTolerant.cpp 
