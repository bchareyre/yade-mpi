#!/bin/zsh
#set -x

if [ "$1" = "" ]; then
  echo you must specify the installation YADE_QMAKE_PATH, for example /usr/local or /home/joeuser/YADE
  exit
fi

echo Installation path: $1
echo please edit _build_/_cache_/default.cache.py to set CXXFLAGS, etc...

ERSKINE=./erskine2.py
YADE=../
export YADE_QMAKE_PATH=$1 YADECOMPILATIONPATH=$YADE

# erskine2.py syntax:
### project-file buildscript-dir > buildscript
# projects are traversed recursively and corresponding waf wscript_build files are sent to the standard output

$ERSKINE $YADE/yade-libs/*/src/*.pro $YADE/yade-libs > $YADE/yade-libs/wscript_build
$ERSKINE $YADE/yade-core/src/yade.pro $YADE/yade-core > $YADE/yade-core/wscript_build
$ERSKINE $YADE/yade-guis/yade-gui-qt/src/QtGUI.pro $YADE/yade-guis > $YADE/yade-guis/wscript_build

# packages are have separated buildscripts each
$ERSKINE $YADE/yade-packages/yade-package-common/src/yade-package-common.pro $YADE/yade-packages/yade-package-common > $YADE/yade-packages/yade-package-common/wscript_build
$ERSKINE $YADE/yade-packages/yade-package-dem/src/yade-package-dem.pro $YADE/yade-packages/yade-package-dem > $YADE/yade-packages/yade-package-dem/wscript_build
$ERSKINE $YADE/yade-packages/yade-package-fem/src/yade-package-fem.pro $YADE/yade-packages/yade-package-fem > $YADE/yade-packages/yade-package-fem/wscript_build
$ERSKINE $YADE/yade-packages/yade-package-lattice/src/yade-package-lattice.pro $YADE/yade-packages/yade-package-lattice > $YADE/yade-packages/yade-package-lattice/wscript_build
$ERSKINE $YADE/yade-packages/yade-package-mass-spring/src/yade-package-mass-spring.pro $YADE/yade-packages/yade-package-mass-spring > $YADE/yade-packages/yade-package-mass-spring/wscript_build
$ERSKINE $YADE/yade-packages/yade-package-realtime-rigidbody/src/yade-package-realtime-rigidbody.pro $YADE/yade-packages/yade-package-realtime-rigidbody > $YADE/yade-packages/yade-package-realtime-rigidbody/wscript_build

