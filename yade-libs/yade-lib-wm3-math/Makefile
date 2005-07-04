# directory tree prefix:
PREFIX_DIR=/

# by default yade is installed in local/
INSTALL_DIR=/usr/local

#######################

yade_include_subdir=yade-lib-wm3-math
yade_lib_subdir=yade-libs


all:
	cd projects/kdevelop ; qmake && make

clean:
	rm bin/*
	find ./projects/kdevelop -name "Makefile" -exec rm {} \;
	find ./projects/kdevelop -name "*.o" -exec rm {} \;

install:
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	mkdir --parents ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	cp bin/* ${PREFIX_DIR}/${INSTALL_DIR}/lib/yade/${yade_lib_subdir}
	cp src/*.hpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/*.ipp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}
	-cp src/*.tpp ${PREFIX_DIR}/${INSTALL_DIR}/include/yade/${yade_include_subdir}


