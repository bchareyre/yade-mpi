isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/home/janek/YADE, this will not work from inside kdevelop (when they will fix it?)" )
}


SUBDIRS += yade 
MOC_DIR = $(YADECOMPILATIONPATH)
UI_DIR = $(YADECOMPILATIONPATH)
OBJECTS_DIR = $(YADECOMPILATIONPATH)
DEFINES = DEBUG
CONFIG += debug \
          thread \
warn_on
TEMPLATE = subdirs
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
