isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
win32 {
TARGET = ../../../bin/XMLFormatManager 
CONFIG += console
}
!win32 {
TARGET = ../../bin/XMLFormatManager 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += XMLFormatManager.hpp \
           XMLSaxParser.hpp 
SOURCES += XMLFormatManager.cpp \
           XMLSaxParser.cpp 
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
