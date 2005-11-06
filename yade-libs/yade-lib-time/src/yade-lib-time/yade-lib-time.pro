isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


win32 {
TARGET = ../../../../bin/yade-lib-time 
CONFIG += console
}
!win32 {
TARGET = ../../../bin/yade-lib-time 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += Chrono.hpp 
SOURCES += Chrono.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

