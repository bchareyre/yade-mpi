isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lMacroMicroElasticRelationships \
        -lBodyMacroParameters \
        -lCundallNonViscousMomentumDamping \
        -lCundallNonViscousForceDamping \
        -lPhysicalActionDamper \
        -lInteractionPhysicsMetaEngine \
        -rdynamic \
        -lGravityEngine \
        -lSphere \
        -lyade-lib-multimethods \
        -lyade-lib-factory \
        -lyade-lib-serialization \
        -lyade-lib-wm3-math 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-package-dem \
               $${YADE_QMAKE_PATH}/lib/yade/yade-package-common \
               $${YADE_QMAKE_PATH}/lib/yade/yade-libs 
win32 {
TARGET = ../../../bin/SphericalDEMSimulator 
CONFIG += console
}
!win32 {
TARGET = ../../bin/SphericalDEMSimulator 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += Contact.hpp \
           PersistentAloneSAPCollider.hpp \
           SphericalDEMSimulator.hpp \
           SphericalDEM.hpp 
SOURCES += PersistentAloneSAPCollider.cpp \
           SphericalDEMSimulator.cpp 
