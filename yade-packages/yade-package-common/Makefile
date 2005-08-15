# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=yade-package-common
yade_package_subdir=yade-package-common
yade_libnames_basename=yade-package-common

all:
	cd projects/kdevelop ; qmake && $(MAKE)

clean:
	-rm bin/*
	-find ./projects/kdevelop -name "Makefile" -exec rm {} \;
	-find ./projects/kdevelop -name "*.o" -exec rm {} \;

install:
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_package_subdir}
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	cp bin/* ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_package_subdir}
	find . -name "*.hpp" -exec cp {}  ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir} \;

uninstall:
	rm -f ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_package_subdir}/*
	rm -rf  ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_package_subdir}
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade
	rmdir --ignore-fail-on-non-empty ${PREFIX_DIR}/${INSTALL_DIR}/include/yade

