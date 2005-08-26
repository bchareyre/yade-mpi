# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=QtGUI
yade_gui_subdir=yade-guis
yade_libnames_basename=QtGUI

all:
	@echo === some dirty replaces are going on. If it will not be foolproof
	@echo === enough, let me know. And I will try to make it better
	@echo === I am doing that, because I want qmake to respect PREFIX_DIR and INSTALL_DIR
	
	find . -name "*.pro" -print | \
		while read name; \
			do grep -l /usr/local $${name}; \
		done | \
		while read name2; \
			do mv $${name2} $${name2}.tmp; \
			cat $${name2}.tmp | sed -e "s/\/usr\/local/$(subst /,\/,$(subst //,/,$(subst //,/,${PREFIX_DIR}/${INSTALL_DIR})))/" > $${name2}; \
			rm $${name2}.tmp; \
		done
	
	cd projects/kdevelop ; qmake && $(MAKE)
	
	find . -name "*.pro" -print | \
		while read name; \
			do grep -l $(subst //,/,$(subst //,/,${PREFIX_DIR}/${INSTALL_DIR})) $${name}; \
		done | while read name2; \
			do mv $${name2} $${name2}.tmp; \
			cat $${name2}.tmp | sed -e "s/$(subst /,\/,$(subst //,/,$(subst //,/,${PREFIX_DIR}/${INSTALL_DIR})))/\/usr\/local/" > $${name2}; \
			rm $${name2}.tmp; \
		done

clean:
	-rm bin/*
	-find ./projects/kdevelop -name "Makefile" -exec rm {} \;
	-find ./projects/kdevelop -name "*.o" -exec rm {} \;
	-find ./projects/kdevelop -name "*.kdevses" -exec rm {} \;
	-find ./projects/kdevelop -name "*.kdevelop.pcs" -exec rm {} \;
	-rm ./projects/kdevelop/QtGUI/QtFileGeneratorController.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedPreferencesEditor.h
	-rm ./projects/kdevelop/QtGUI/YadeQtGeneratedMainWindow.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedCodeGenerator.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedEngineEditor.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedCodeGenerator.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSimulationController.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedPreferencesEditor.cpp
	-rm ./projects/kdevelop/QtGUI/moc_GLEngineEditor.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedMessageDialog.h
	-rm ./projects/kdevelop/QtGUI/moc_QtFileGeneratorController.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedMetaDispatchingEngineProperties.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedEngineEditor.cpp
	-rm ./projects/kdevelop/QtGUI/QtFileGeneratorController.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedCodeGenerator.cpp
	-rm ./projects/kdevelop/QtGUI/YadeQtGeneratedMainWindow.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSimulationController.cpp
	-rm ./projects/kdevelop/QtGUI/moc_GLViewer.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedMessageDialog.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedMetaDispatchingEngineProperties.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedEngineEditor.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedSimulationController.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedMessageDialog.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedMetaDispatchingEngineProperties.cpp
	-rm ./projects/kdevelop/QtGUI/moc_YadeQtGeneratedMainWindow.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedPreferencesEditor.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedSphericalDEMSimulator.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSphericalDEMSimulator.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSphericalDEMSimulator.cpp
	-rm ./projects/kdevelop/QtGUI/moc_QtGeneratedSimulationPlayer.cpp
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSimulationPlayer.h
	-rm ./projects/kdevelop/QtGUI/QtGeneratedSimulationPlayer.cpp

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

