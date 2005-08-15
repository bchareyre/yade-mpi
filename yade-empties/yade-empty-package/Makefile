# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=
yade_package_subdir=
yade_libnames_basename=

all:
	@echo === some dirty replaces are going on. If it will not be foolproof
	@echo === enough, let me know. And I will try to make it better
	@echo === I am doing that, because I want qmake to respect PREFIX_DIR and INSTALL_DIR
	
	find . -name "*.pro" -print | while read name; do grep -l /usr/local $${name}; done | while read name2; do mv $${name2} $${name2}.tmp; cat $${name2}.tmp | sed -e "s/\/usr\/local/\${PREFIX_DIR}\/\${INSTALL_DIR}/" > $${name2}; rm $${name2}.tmp; done
	cd projects/kdevelop ; qmake && $(MAKE)
	find . -name "*.pro" -print | while read name; do grep -l ${PREFIX_DIR}/${INSTALL_DIR} $${name}; done | while read name2; do mv $${name2} $${name2}.tmp; cat $${name2}.tmp | sed -e "s/\${PREFIX_DIR}\/\${INSTALL_DIR}/\/usr\/local/" > $${name2}; rm $${name2}.tmp; done

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

