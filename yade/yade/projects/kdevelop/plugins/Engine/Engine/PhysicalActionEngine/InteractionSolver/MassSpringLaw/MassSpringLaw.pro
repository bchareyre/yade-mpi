# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/Engine/Engine/PhysicalActionEngine/InteractionSolver/MassSpringLaw
# Target is a library:  

LIBS += -lEngine \
        -lBody \
        -lyade-lib-serialization \
        -lMesh2D \
        -lyade-lib-wm3-math \
        -lInteraction \
        -lyade-lib-multimethods \
        -lSpringPhysics \
        -lSpringGeometry \
        -lParticleParameters \
        -lForce \
        -lMomentum \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../../../yade/Engine/$(YADEDYNLIBPATH) \
               ../../../../../../yade/Body/Body/$(YADEDYNLIBPATH) \
               ../../../../../../toolboxes/Libraries/yade-lib-serialization/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Body/GeometricalModel/Mesh2D/$(YADEDYNLIBPATH) \
               ../../../../../../toolboxes/Libraries/yade-lib-wm3-math/$(YADEDYNLIBPATH) \
               ../../../../../../yade/Interaction/Interaction/$(YADEDYNLIBPATH) \
               ../../../../../../toolboxes/Libraries/yade-lib-multimethods/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/InteractionPhysics/SpringPhysics/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Interaction/NarrowInteractionGeometry/SpringGeometry/$(YADEDYNLIBPATH) \
               ../../../../../../plugins/Data/Body/PhysicalParameters/ParticleParameters/$(YADEDYNLIBPATH) \
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
HEADERS += MassSpringLaw.hpp 
SOURCES += MassSpringLaw.cpp 
