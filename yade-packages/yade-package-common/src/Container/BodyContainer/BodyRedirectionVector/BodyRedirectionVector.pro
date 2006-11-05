isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


HEADERS += BodyRedirectionVector.hpp \
           BodyRedirectionVectorIterator.hpp 
SOURCES += BodyRedirectionVector.cpp \
           BodyRedirectionVectorIterator.cpp 
LIBS += -rdynamic
QMAKE_CXXFLAGS_RELEASE += -lpthread \
-pthread
QMAKE_CXXFLAGS_DEBUG += -lpthread \
-pthread
DESTDIR = ../../../../bin
CONFIG += debug \
          thread \
warn_on \
dll
TEMPLATE = lib
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
