isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


HEADERS += DynLibDispatcher.hpp \
           FunctorWrapper.hpp \
           Indexable.hpp \
           MultiMethodsExceptions.hpp 
SOURCES += Indexable.cpp \
           MultiMethodsExceptions.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include
win32 {
TARGET = ../../../bin/yade-lib-multimethods
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-multimethods
}

CONFIG += debug \
          thread \
warn_on \
dll
TEMPLATE = lib
