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

compile_install:
	$(MAKE) -C yade-libs compile_install
	
	$(MAKE) -C yade-core
	$(MAKE) -C yade-core install
	
	$(MAKE) -C yade-guis
	$(MAKE) -C yade-guis install
	
	$(MAKE) -C yade-packages compile_install

uninstall:
	$(MAKE) -C yade-libs uninstall
	$(MAKE) -C yade-core uninstall
	$(MAKE) -C yade-guis uninstall
	$(MAKE) -C yade-packages uninstall
