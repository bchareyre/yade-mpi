isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}

HEADERS += Archive.hpp \
           ArchiveTypes.hpp \
           IOManagerExceptions.hpp \
           IOFormatManager.hpp \
           Serializable.hpp \
           SerializableSingleton.hpp \
           SerializableTypes.hpp \
           SerializationExceptions.hpp \
           Archive.tpp \
           ContainerHandler.tpp \
           FundamentalHandler.tpp \
           IOFormatManager.tpp \
           KnownFundamentalsHandler.tpp \
           MultiTypeHandler.tpp \
           PointerHandler.tpp 
SOURCES += Archive.cpp \
           IOFormatManager.cpp \
           IOManagerExceptions.cpp \
           Serializable.cpp \
           SerializableSingleton.cpp \
           SerializationExceptions.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include
win32 {
TARGET = ../../../bin/yade-lib-serialization
CONFIG += console
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/

LIBS += -llibboost_thread-mgw-mt-d-1_33 \
        -llibyade-lib-factory

#-lClassFactory

}
!win32 {
TARGET = ../../bin/yade-lib-serialization
}

CONFIG += debug \
          thread \
warn_on \
dll
TEMPLATE = lib
