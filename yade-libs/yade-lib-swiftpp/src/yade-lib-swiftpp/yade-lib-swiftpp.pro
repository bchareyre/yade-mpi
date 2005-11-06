isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lyade-lib-qhull 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
win32 {
TARGET = ../../../bin/yade-lib-swiftpp 
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-swiftpp 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += SWIFT_array.h \
           SWIFT_boxnode.h \
           SWIFT_common.h \
           SWIFT_config.h \
           SWIFT_fileio.h \
           SWIFT_front.h \
           SWIFT.h \
           SWIFT_linalg.h \
           SWIFT_lut.h \
           SWIFT_mesh.h \
           SWIFT_mesh_utils.h \
           SWIFT_object.h \
           SWIFT_pair.h \
           SWIFT_pqueue.h 
SOURCES += fileio.cpp \
           lut.cpp \
           mesh.cpp \
           mesh_utils.cpp \
           object.cpp \
           pair.cpp \
           pqueue.cpp \
           scene.cpp 
