isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


win32 {
TARGET = ../../../../bin/yade-lib-wm3-math 
CONFIG += console
}
!win32 {
TARGET = ../../../bin/yade-lib-wm3-math 
}

CONFIG += debug \
          thread \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += Math.ipp \
           Matrix2.ipp \
           Matrix3.ipp \
           Matrix4.ipp \
           Quaternion.ipp \
           Se3.ipp \
           Vector2.ipp \
           Vector3.ipp \
           Vector4.ipp \
           Math.hpp \
           Matrix2.hpp \
           Matrix3.hpp \
           Matrix4.hpp \
           Quaternion.hpp \
           Se3.hpp \
           Vector2.hpp \
           Vector3.hpp \
           Vector4.hpp 
SOURCES += Math.cpp \
           Matrix2.cpp \
           Matrix3.cpp \
           Matrix4.cpp \
           Quaternion.cpp \
           Se3.cpp \
           Vector2.cpp \
           Vector3.cpp \
           Vector4.cpp 
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

