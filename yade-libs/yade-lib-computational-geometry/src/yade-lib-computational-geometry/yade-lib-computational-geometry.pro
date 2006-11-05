isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


HEADERS += Distances2D.hpp \
           Distances3D.hpp \
           Intersections2D.hpp \
           Intersections3D.hpp \
           MarchingCube.hpp 
SOURCES += Distances2D.cpp \
           Distances3D.cpp \
           Intersections2D.cpp \
           Intersections3D.cpp \
           MarchingCube.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include
win32 {
TARGET = ../../../bin/yade-lib-computational-geometry
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-computational-geometry
}

CONFIG += debug \
          thread \
warn_on \
dll
TEMPLATE = lib
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
