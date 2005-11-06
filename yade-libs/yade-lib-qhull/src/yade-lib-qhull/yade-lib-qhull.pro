isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


win32 {
TARGET = ../../../bin/yade-lib-qhull 
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-qhull 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += geom.h \
           io.h \
           mem.h \
           merge.h \
           poly.h \
           qhull_a.h \
           qhull.h \
           qset.h \
           stat.h \
           user.h 
SOURCES += geom2.c \
           geom.c \
           global.c \
           io.c \
           mem.c \
           merge.c \
           poly2.c \
           poly.c \
           qhull.c \
           qset.c \
           stat.c \
           unix.c \
           user.c 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

