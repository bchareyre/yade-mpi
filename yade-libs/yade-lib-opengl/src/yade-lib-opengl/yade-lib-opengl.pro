isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


LIBS += -lglut
INCLUDEPATH += $${YADE_QMAKE_PATH}/include 
QMAKE_LIBDIR = $${YADE_QMAKE_PATH}/lib/yade/yade-libs/ 
win32 {
TARGET = ../../../bin/yade-lib-opengl 
CONFIG += console
}
!win32 {
TARGET = ../../bin/yade-lib-opengl 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += FpsTracker.hpp \
           GLTextLabel.hpp \
           GLWindow.hpp \
           GLWindowsManager.hpp \
           OpenGLWrapper.hpp 
SOURCES += FpsTracker.cpp \
           GLTextLabel.cpp \
           GLWindow.cpp \
           GLWindowsManager.cpp 
