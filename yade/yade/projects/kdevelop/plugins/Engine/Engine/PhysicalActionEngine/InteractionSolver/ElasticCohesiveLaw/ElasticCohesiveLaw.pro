# Plik utworzony przez menad?era projektów kdevelopa
# ------------------------------------------- 
# Podkatalog wzgl?dem g?ównego katalogu projektu: ./plugins/Engine/Engine/PhysicalActionEngine/InteractionSolver/ElasticCohesiveLaw
# Cel to biblioteka 

LIBS += -lyade-lib-serialization \
        -lSphere \
        -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lSDECLinkGeometry \
        -lSDECLinkPhysics \
        -lRigidBodyParameters \
        -lElasticContactParameters \
        -lMacroMicroContactGeometry \
        -lBodyMacroParameters \
        -lForce \
        -lMomentum \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../../../libraries/yade-lib-serialization/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Body/GeometricalModel/Sphere/$(YADEDYNLIBPATH) \
               ../../../../../../libraries/yade-lib-wm3-math/$(YADEDYNLIBPATH) \
               ../../../../../../libraries/yade-lib-multimethods/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/NarrowInteractionGeometry/SDECLinkGeometry/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/InteractionPhysics/SDECLinkPhysics/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Body/PhysicalParameters/RigidBodyParameters/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/InteractionPhysics/ElasticContactParameters/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/NarrowInteractionGeometry/MacroMicroContactGeometry/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Body/PhysicalParameters/BodyMacroParameters/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/PhysicalAction/Force/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/PhysicalAction/Momentum/$(YADEDYNLIBPATH) \
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
HEADERS += ElasticCohesiveLaw.hpp 
SOURCES += ElasticCohesiveLaw.cpp 
