isEmpty ( YADE_QMAKE_PATH ) {
error( "YADE_QMAKE_PATH internal qmake variable is not set, you should run for example qmake YADE_QMAKE_PATH=/usr/local, this will not work from inside kdevelop (when they will fix it?)" )
}


SUBDIRS += MacroMicroElasticRelationships \
           InteractingBox2InteractingSphere4SpheresContactGeometry \
           InteractingSphere2InteractingSphere4SpheresContactGeometry \
	   InteractingMyTetrahedron2AABB \
	   InteractingMyTetrahedron2InteractingMyTetrahedron4InteractionOfMyTetrahedron \
	   InteractingMyTetrahedron2InteractingBox4InteractionOfMyTetrahedron \
	   Tetrahedron2InteractingMyTetrahedron
CONFIG += debug \
          thread \
warn_on
TEMPLATE = subdirs
INCLUDEPATH += $${YADE_QMAKE_PATH}/include

QMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $(shell pwd)/$<
