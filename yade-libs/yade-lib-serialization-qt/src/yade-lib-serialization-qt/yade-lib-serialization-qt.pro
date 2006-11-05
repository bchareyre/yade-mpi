isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lXMLFormatManager 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
win32 {
TARGET = ../../../bin/yade-lib-serialization-qt 
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-serialization-qt 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += QtGUIGenerator.hpp 
SOURCES += QtGUIGenerator.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
