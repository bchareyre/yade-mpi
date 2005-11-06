isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=\/usr\/local, this will not work from inside kdevelop (when they will fix it?)" )
}

win32 {
TARGET = ../../../bin/yade-lib-loki 
CONFIG += console
}

!win32 {
TARGET = ../../bin/yade-lib-loki 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += AssocVector.hpp \
           EmptyType.hpp \
           Functor.hpp \
           NullType.hpp \
           Singleton.hpp \
           Typelist.hpp \
           TypeManip.hpp \
           TypeTraits.hpp 

INCLUDEPATH += $${YADE_QMAKE_PATH}/include
