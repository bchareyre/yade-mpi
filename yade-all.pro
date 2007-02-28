isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/home/janek/YADE, this will not work from inside kdevelop (when they will fix it?)" )
}


SUBDIRS += \
yade-core/src yade-libs/yade-lib-base/src \
yade-libs/yade-lib-computational-geometry/src \
yade-libs/yade-lib-factory/src \
yade-libs/yade-lib-loki/src \
yade-libs/yade-lib-multimethods/src \
yade-libs/yade-lib-opengl/src \
yade-libs/yade-lib-serialization-bin/src \
yade-libs/yade-lib-serialization-qt/src \
yade-libs/yade-lib-serialization/src \
yade-libs/yade-lib-serialization-xml/src \
yade-guis/yade-gui-qt/src \
yade-extra/yade-spherical-dem-simulator/src \
yade-packages/yade-package-common/src \
yade-packages/yade-package-dem/src \
yade-packages/yade-package-fem/src \
yade-packages/yade-package-lattice/src \
yade-packages/yade-package-mass-spring/src \
yade-packages/yade-package-realtime-rigidbody/src

#yade-core/src/yade.pro
#yade-libs/yade-lib-base/src/yade-lib-base.pro
#yade-libs/yade-lib-computational-geometry/src/yade-lib-computational-geometry.pro
#yade-libs/yade-lib-factory/src/yade-lib-factory.pro
#yade-libs/yade-lib-loki/src/yade-lib-loki.pro
#yade-libs/yade-lib-multimethods/src/yade-lib-multimethods.pro
#yade-libs/yade-lib-opengl/src/yade-lib-opengl.pro
#yade-libs/yade-lib-serialization-bin/src/yade-lib-serialization-bin.pro
#yade-libs/yade-lib-serialization-qt/src/yade-lib-serialization-qt.pro
#yade-libs/yade-lib-serialization/src/yade-lib-serialization.pro
#yade-libs/yade-lib-serialization-xml/src/yade-lib-serialization-xml.pro
#yade-guis/yade-gui-qt/src/QtGUI.pro
#yade-extra/yade-spherical-dem-simulator/src/yade-spherical-dem-simulator.pro
#yade-packages/yade-package-common/src/yade-package-common.pro
#yade-packages/yade-package-dem/src/yade-package-dem.pro
#yade-packages/yade-package-fem/src/yade-package-fem.pro
#yade-packages/yade-package-lattice/src/yade-package-lattice.pro
#yade-packages/yade-package-mass-spring/src/yade-package-mass-spring.pro
#yade-packages/yade-package-realtime-rigidbody/src/yade-package-realtime-rigidbody.pro

MOC_DIR = $(YADECOMPILATIONPATH)
UI_DIR = $(YADECOMPILATIONPATH)
OBJECTS_DIR = $(YADECOMPILATIONPATH)
DEFINES = DEBUG
CONFIG += debug thread warn_on
TEMPLATE = subdirs
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
