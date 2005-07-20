all:
	@echo --
	@echo Please invoke \'make compile_install\' because \'make\' itself
	@echo is unable to do everything needed. See INSTALL for details
	@echo --

clean:
	$(MAKE) -C yade-package-common clean
	$(MAKE) -C yade-package-dem clean
	$(MAKE) -C yade-package-fem clean
	$(MAKE) -C yade-package-lattice clean
	$(MAKE) -C yade-package-mass-spring clean
	$(MAKE) -C yade-package-realtime-rigidbody clean


compile_install:
	
	$(MAKE) -C yade-package-common 
	$(MAKE) -C yade-package-common install
	
	$(MAKE) -C yade-package-dem 
	$(MAKE) -C yade-package-dem install

	$(MAKE) -C yade-package-fem 
	$(MAKE) -C yade-package-fem install
	
	$(MAKE) -C yade-package-lattice
	$(MAKE) -C yade-package-lattice install


	$(MAKE) -C yade-package-mass-spring
	$(MAKE) -C yade-package-mass-spring install
	
	$(MAKE) -C yade-package-realtime-rigidbody
	$(MAKE) -C yade-package-realtime-rigidbody install
	
uninstall:
	$(MAKE) -C yade-package-common uninstall
	$(MAKE) -C yade-package-dem uninstall
	$(MAKE) -C yade-package-fem uninstall
	$(MAKE) -C yade-package-lattice uninstall
	$(MAKE) -C yade-package-mass-spring uninstall
	$(MAKE) -C yade-package-realtime-rigidbody uninstall
