isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}

INCLUDEPATH += $${YADE_QMAKE_PATH}/include/
win32 {
TARGET = ../../../bin/yade-lib-base
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-base
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib
HEADERS += yadeWm3.hpp \
           yadeWm3Extra.hpp \
			  Logging.hpp
SOURCES += yadeWm3Extra.cpp
LIBS += -lWm3Foundation -rdynamic
QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
