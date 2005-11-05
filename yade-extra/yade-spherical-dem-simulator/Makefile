# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=yade-extra
yade_gui_subdir=yade-extra
yade_libnames_basename=SphericalDEMSimulator

all:
	cd projects/kdevelop ; qmake "YADE_QMAKE_PATH=${PREFIX_DIR}/${INSTALL_DIR}" && $(MAKE)

clean:
	-rm bin/*
	-find ./projects/kdevelop -name "Makefile" -exec rm {} \;
	-find ./projects/kdevelop -name "*.o" -exec rm {} \;
	-find ./projects/kdevelop -name "*.kdevses" -exec rm {} \;
	-find ./projects/kdevelop -name "*.kdevelop.pcs" -exec rm {} \;

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

