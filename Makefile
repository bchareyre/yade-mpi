all:
	@echo --
	@echo Please invoke \'make compile_install\' because \'make\' itself
	@echo is unable to do everything needed. See INSTALL for details
	@echo --

clean:
	$(MAKE) -C yade-libs clean
	$(MAKE) -C yade-core clean
	$(MAKE) -C yade-guis clean
	$(MAKE) -C yade-packages clean
	$(MAKE) -C yade-extra clean
	rm -rf config.log .sconf_temp include .sconsign.dblite # scons.config should be kept alone
	find -name ".sconsign" -exec rm -rf {} \;
	find -name "SConscript" -exec rm -rf {} \;

compile_install:
	$(MAKE) -C yade-libs compile_install
	
	$(MAKE) -C yade-core
	$(MAKE) -C yade-core install
	
	$(MAKE) -C yade-guis
	$(MAKE) -C yade-guis install
	
	$(MAKE) -C yade-packages compile_install
	
	$(MAKE) -C yade-extra
	$(MAKE) -C yade-extra install
	
	@echo --
	@echo Compilation and installation finished.
	@echo --

uninstall:
	$(MAKE) -C yade-libs uninstall
	$(MAKE) -C yade-core uninstall
	$(MAKE) -C yade-guis uninstall
	$(MAKE) -C yade-packages uninstall
	$(MAKE) -C yade-extra uninstall

tags:
	# prevent generated symlinked files from yade-flat and include to be parsed
	ctags -R --extra=+q --fields=+n --exclude='.*' --exclude=yade-flat --exclude=include --exclude='*.so'

doc:
	cd yade-doc; doxygen Doxyfile

#### convenience and individual targets; saves typing.
# helper target for parallel compilation which is broken generally and needs to be tried repeatedly until it succeeds (or not)
ci:
	$(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install || $(MAKE) compile_install

eudoxos:
	$(MAKE) ci -j4 INSTALL_DIR='/YADE' PREFIX_DIR=/tmp CXX="distcc g++-4.0" LINK="g++-4.0" CXXFLAGS="-I/usr/local/include/wm3"

