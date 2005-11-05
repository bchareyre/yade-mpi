isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}

 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
win32 {
TARGET = ../../../../bin/yade-empty-gui
CONFIG += console
}
!win32 {
TARGET = ../../../bin/yade-empty-gui
}

CONFIG += debug \
          thread \
          warn_on \
          dll
TEMPLATE = lib
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

