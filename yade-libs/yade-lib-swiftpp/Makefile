# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=yade-lib-swiftpp
yade_lib_subdir=yade-libs
yade_libnames_basename=yade-lib-swiftpp

all:
	cd src && qmake "YADE_QMAKE_PATH=${PREFIX_DIR}/${INSTALL_DIR}" && $(MAKE)

clean:
	-rm bin/*
	-find ./src -name "Makefile" -exec rm {} \;
	-find ./src -name "*.o" -exec rm {} \;

install:
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	cp --no-dereference bin/* ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	cp src/${yade_include_subdir}/*.h ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}

uninstall:
	rm -f ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}/lib${yade_libnames_basename}*
	rm -rf  ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/include/yade

