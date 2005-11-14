all:
	@echo --
	@echo Please invoke \'make compile_install\' because \'make\' itself
	@echo is unable to do everything needed. See INSTALL for details
	@echo --

clean:
	$(MAKE) -C yade-lib-loki clean
	$(MAKE) -C yade-lib-wm3-math clean
	$(MAKE) -C yade-lib-algorithms clean
	$(MAKE) -C yade-lib-factory clean
	$(MAKE) -C yade-lib-multimethods clean
	$(MAKE) -C yade-lib-serialization clean
	$(MAKE) -C yade-lib-threads clean
	$(MAKE) -C yade-lib-computational-geometry clean
	$(MAKE) -C yade-lib-time clean
	$(MAKE) -C yade-lib-opengl clean
	$(MAKE) -C yade-lib-serialization-xml clean
	$(MAKE) -C yade-lib-serialization-bin clean
	$(MAKE) -C yade-lib-serialization-qt clean
	$(MAKE) -C yade-lib-qhull clean
	$(MAKE) -C yade-lib-swiftpp clean

compile_install:
	$(MAKE) -C yade-lib-loki
	$(MAKE) -C yade-lib-loki install
	
	$(MAKE) -C yade-lib-wm3-math
	$(MAKE) -C yade-lib-wm3-math install
	
	$(MAKE) -C yade-lib-algorithms
	$(MAKE) -C yade-lib-algorithms install
	
	$(MAKE) -C yade-lib-factory
	$(MAKE) -C yade-lib-factory install
	
	$(MAKE) -C yade-lib-multimethods
	$(MAKE) -C yade-lib-multimethods install
	
	$(MAKE) -C yade-lib-serialization
	$(MAKE) -C yade-lib-serialization install
	
	$(MAKE) -C yade-lib-threads
	$(MAKE) -C yade-lib-threads install
	
	$(MAKE) -C yade-lib-computational-geometry
	$(MAKE) -C yade-lib-computational-geometry install
	
	$(MAKE) -C yade-lib-time
	$(MAKE) -C yade-lib-time install
	
	$(MAKE) -C yade-lib-opengl
	$(MAKE) -C yade-lib-opengl install
	
	$(MAKE) -C yade-lib-serialization-xml
	$(MAKE) -C yade-lib-serialization-xml install
	
	$(MAKE) -C yade-lib-serialization-bin
	$(MAKE) -C yade-lib-serialization-bin install
	
	$(MAKE) -C yade-lib-serialization-qt
	$(MAKE) -C yade-lib-serialization-qt install
	
	$(MAKE) -C yade-lib-qhull
	$(MAKE) -C yade-lib-qhull install
	
	$(MAKE) -C yade-lib-swiftpp
	$(MAKE) -C yade-lib-swiftpp install

uninstall:
	$(MAKE) -C yade-lib-loki uninstall
	$(MAKE) -C yade-lib-wm3-math uninstall
	$(MAKE) -C yade-lib-algorithms uninstall
	$(MAKE) -C yade-lib-factory uninstall
	$(MAKE) -C yade-lib-multimethods uninstall
	$(MAKE) -C yade-lib-serialization uninstall
	$(MAKE) -C yade-lib-threads uninstall
	$(MAKE) -C yade-lib-computational-geometry uninstall
	$(MAKE) -C yade-lib-time uninstall
	$(MAKE) -C yade-lib-opengl uninstall
	$(MAKE) -C yade-lib-serialization-xml uninstall
	$(MAKE) -C yade-lib-serialization-bin uninstall
	$(MAKE) -C yade-lib-serialization-qt uninstall
	$(MAKE) -C yade-lib-qhull uninstall
	$(MAKE) -C yade-lib-swiftpp uninstall

