# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./preprocessors/FileGenerator/FEMDEMCouplingTest
# Target is a library:  

LIBS += -lAABB \
        -lFEMSetParameters \
        -lPhysicalActionInitializer \
        -lFEMSetTextLoader \
        -lFEMLaw \
        -lBox \
        -lInteractingBox \
        -lBodyMacroParameters \
        -lPersistentSAPCollider \
        -lSDECTimeStepper \
        -lCundallNonViscousForceDamping \
        -lCundallNonViscousMomentumDamping \
        -lElasticContactLaw \
        -lMetaInteractingGeometry \
        -lPhysicalActionReseter \
        -lGravityEngine \
        -lPhysicalActionVectorVector \
        -lInteractionVecSet \
        -lBodyRedirectionVector \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../plugins/Data/Body/BoundingVolume/AABB/$(YADEDYNLIBPATH) \
               ../../../plugins/Data/Body/PhysicalParameters/FEMSetParameters/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Engine/PhysicalActionEngine/PhysicalActionContainerInitializer/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/EngineUnit/BodyEngineUnit/PhysicalParametersEngineUnit/FEMSetTextLoader/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Engine/PhysicalActionEngine/InteractionSolver/FEMLaw/$(YADEDYNLIBPATH) \
               ../../../plugins/Data/Body/GeometricalModel/Box/$(YADEDYNLIBPATH) \
               ../../../plugins/Data/Body/InteractingGeometry/InteractingBox/$(YADEDYNLIBPATH) \
               ../../../plugins/Data/Body/PhysicalParameters/BodyMacroParameters/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Engine/InteractionEngine/BroadInteractionEngine/PersistentSAPCollider/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/TimeStepper/SDECTimeStepper/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/EngineUnit/PhysicalActionEngineUnit/CundallNonViscousForceDamping/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/EngineUnit/PhysicalActionEngineUnit/CundallNonViscousMomentumDamping/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Engine/PhysicalActionEngine/InteractionSolver/ElasticContactLaw/$(YADEDYNLIBPATH) \
               ../../../plugins/Data/Body/InteractingGeometry/MetaInteractingGeometry/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/Engine/PhysicalActionEngine/PhysicalActionContainerReseter/$(YADEDYNLIBPATH) \
               ../../../plugins/Engine/DeusExMachina/GravityEngine/$(YADEDYNLIBPATH) \
               ../../../plugins/Container/PhysicalActionContainer/ActionParameterVectorVector/$(YADEDYNLIBPATH) \
               ../../../plugins/Container/InteractionContainer/InteractionVecSet/$(YADEDYNLIBPATH) \
               ../../../plugins/Container/BodyContainer/BodyRedirectionVector/$(YADEDYNLIBPATH) \
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
HEADERS += FEMDEMCouplingTest.hpp 
SOURCES += FEMDEMCouplingTest.cpp 
