# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=yade-lib-serialization-qt
yade_lib_subdir=yade-libs
yade_libnames_basename=yade-lib-serialization-qt

all:
	cd src && qmake "YADE_QMAKE_PATH=${PREFIX_DIR}/${INSTALL_DIR}" && $(MAKE)

clean:
	-rm bin/*
	-rm ./src/yade-lib-serialization-qt/moc_QtGUIGenerator.cpp
	-find ./src -name "Makefile" -exec rm {} \;
	-find ./src -name "*.o" -exec rm {} \;

install:
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	cp --no-dereference bin/* ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	cp src/${yade_include_subdir}/*.hpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/${yade_include_subdir}/*.ipp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/${yade_include_subdir}/*.tpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}

uninstall:
	rm -f ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}/lib${yade_libnames_basename}*
	rm -rf  ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/include/yade

