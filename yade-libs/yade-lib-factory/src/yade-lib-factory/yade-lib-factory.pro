isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
win32 {
TARGET = ../../../bin/yade-lib-factory 
CONFIG += console
LIBS += -llibboost_filesystem-mgw-mt-d-1_33 \
        -llibboost_thread-mgw-mt-d-1_33
}
!win32 {
TARGET = ../../bin/yade-lib-factory 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += ClassFactory.hpp \
           DynLibManager.hpp \
           Factorable.hpp \
           FactoryExceptions.hpp 
SOURCES += ClassFactory.cpp \
           DynLibManager.cpp \
           FactoryExceptions.cpp 
