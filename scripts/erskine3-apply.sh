#!/bin/sh
#set -x


ERSKINE=./erskine3.py
YADE=../
# erskine3 accepts scons variables smuggled as @VARIABLE and resulting in $VARIABLE in generated scripts; do that for $PREFIX (ac scons var)
export YADE_QMAKE_PATH=@PREFIX YADECOMPILATIONPATH=$YADE
ENGINE=--scons; SCRIPT=SConscript
#ENGINE=--waf; SCRIPT=wscript_build

# erskine2.py syntax:
### project-file [--waf|--scons] buildscript-dir > buildscript
# projects are traversed recursively and corresponding waf wscript_build files are sent to the standard output

$ERSKINE $ENGINE $YADE/yade-libs/*/src/*.pro $YADE/yade-libs > $YADE/yade-libs/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-core/src/yade.pro $YADE/yade-core > $YADE/yade-core/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-guis/yade-gui-qt/src/QtGUI.pro $YADE/yade-guis > $YADE/yade-guis/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-extra/yade-spherical-dem-simulator/src/yade-spherical-dem-simulator.pro $YADE/yade-extra > $YADE/yade-extra/$SCRIPT

# packages are have separated buildscripts each
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-common/src/yade-package-common.pro $YADE/yade-packages/yade-package-common > $YADE/yade-packages/yade-package-common/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-dem/src/yade-package-dem.pro $YADE/yade-packages/yade-package-dem > $YADE/yade-packages/yade-package-dem/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-fem/src/yade-package-fem.pro $YADE/yade-packages/yade-package-fem > $YADE/yade-packages/yade-package-fem/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-lattice/src/yade-package-lattice.pro $YADE/yade-packages/yade-package-lattice > $YADE/yade-packages/yade-package-lattice/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-mass-spring/src/yade-package-mass-spring.pro $YADE/yade-packages/yade-package-mass-spring > $YADE/yade-packages/yade-package-mass-spring/$SCRIPT
$ERSKINE $ENGINE $YADE/yade-packages/yade-package-realtime-rigidbody/src/yade-package-realtime-rigidbody.pro $YADE/yade-packages/yade-package-realtime-rigidbody > $YADE/yade-packages/yade-package-realtime-rigidbody/$SCRIPT

echo Creating entry for yade-extra/Clump in $YADE/yade-extra/$SCRIPT
echo "env.SConscript(dirs=['clump'])" >> $YADE/yade-extra/$SCRIPT
