# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=QtGUI
yade_gui_subdir=yade-guis
yade_libnames_basename=QtGUI

all:
	cd src && qmake "YADE_QMAKE_PATH=${PREFIX_DIR}/${INSTALL_DIR}" && $(MAKE)

clean:
	-rm bin/*
	-find ./src -name "Makefile" -exec rm {} \;
	-find ./src -name "*.o" -exec rm {} \;
	-find ./src -name "*.kdevses" -exec rm {} \;
	-find ./src -name "*.kdevelop.pcs" -exec rm {} \;
	-rm ./src/QtGUI/QtFileGeneratorController.cpp
	-rm ./src/QtGUI/QtGeneratedPreferencesEditor.h
	-rm ./src/QtGUI/YadeQtGeneratedMainWindow.cpp
	-rm ./src/QtGUI/moc_QtGeneratedCodeGenerator.cpp
	-rm ./src/QtGUI/QtGeneratedEngineEditor.h
	-rm ./src/QtGUI/QtGeneratedCodeGenerator.h
	-rm ./src/QtGUI/QtGeneratedSimulationController.h
	-rm ./src/QtGUI/QtGeneratedPreferencesEditor.cpp
	-rm ./src/QtGUI/moc_GLEngineEditor.cpp
	-rm ./src/QtGUI/QtGeneratedMessageDialog.h
	-rm ./src/QtGUI/moc_QtFileGeneratorController.cpp
	-rm ./src/QtGUI/QtGeneratedMetaDispatchingEngineProperties.h
	-rm ./src/QtGUI/QtGeneratedEngineEditor.cpp
	-rm ./src/QtGUI/QtFileGeneratorController.h
	-rm ./src/QtGUI/QtGeneratedCodeGenerator.cpp
	-rm ./src/QtGUI/YadeQtGeneratedMainWindow.h
	-rm ./src/QtGUI/QtGeneratedSimulationController.cpp
	-rm ./src/QtGUI/moc_GLViewer.cpp
	-rm ./src/QtGUI/QtGeneratedMessageDialog.cpp
	-rm ./src/QtGUI/QtGeneratedMetaDispatchingEngineProperties.cpp
	-rm ./src/QtGUI/moc_QtGeneratedEngineEditor.cpp
	-rm ./src/QtGUI/moc_QtGeneratedSimulationController.cpp
	-rm ./src/QtGUI/moc_QtGeneratedMessageDialog.cpp
	-rm ./src/QtGUI/moc_QtGeneratedMetaDispatchingEngineProperties.cpp
	-rm ./src/QtGUI/moc_YadeQtGeneratedMainWindow.cpp
	-rm ./src/QtGUI/moc_QtGeneratedPreferencesEditor.cpp
	-rm ./src/QtGUI/moc_QtGeneratedSphericalDEMSimulator.cpp
	-rm ./src/QtGUI/QtGeneratedSphericalDEMSimulator.h
	-rm ./src/QtGUI/QtGeneratedSphericalDEMSimulator.cpp
	-rm ./src/QtGUI/moc_QtGeneratedSimulationPlayer.cpp
	-rm ./src/QtGUI/QtGeneratedSimulationPlayer.h
	-rm ./src/QtGUI/QtGeneratedSimulationPlayer.cpp

install:
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_gui_subdir}
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	cp --no-dereference bin/* ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_gui_subdir}
	cp src/*.hpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/*.ipp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/*.tpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}

uninstall:
	rm -f ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_gui_subdir}/lib${yade_libnames_basename}*
	rm -rf  ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_gui_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/include/yade

