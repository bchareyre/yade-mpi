all:
	@echo --
	@echo Please invoke \'make install\' because \'make\' itself
	@echo is unable to do everything needed. See INSTALL for details
	@echo --

clean:
	$(MAKE) -C yade-lib-loki clean
	$(MAKE) -C yade-lib-wm3-math clean
	$(MAKE) -C yade-lib-algorithms clean

install:
	$(MAKE) -C yade-lib-loki
	$(MAKE) -C yade-lib-loki install
	$(MAKE) -C yade-lib-wm3-math
	$(MAKE) -C yade-lib-wm3-math install
	$(MAKE) -C yade-lib-algorithms
	$(MAKE) -C yade-lib-algorithms install

uninstall:
	$(MAKE) -C yade-lib-loki uninstall
	$(MAKE) -C yade-lib-wm3-math uninstall
	$(MAKE) -C yade-lib-algorithms uninstall

