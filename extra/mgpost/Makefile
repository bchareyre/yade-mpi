##########################################################################
#  Copyright (C) 2008 by Vincent Richefeu                                #
#  vincent.richefeu@hmg.inpg.fr                                          #
#                                                                        #
#  This program is free software; it is licensed under the terms of the  #
#  GNU General Public License v2 or later. See file LICENSE for details. #
##########################################################################


INSTALL_DIR = /usr/local/bin

default: linux

macosx: src/*.c src/*.h
	${MAKE} -f Makefile.macosx -C src

windows: src/*.c src/*.h
	${MAKE} -f Makefile.windows -C src

linux: src/*.c src/*.h
	${MAKE} -f Makefile.linux -C src

clean:
	rm -f *~ \#*\# ; \
	rm -f ./mgpost ; \
	rm -f ./mgpost.exe ; \
	${MAKE} -f Makefile.linux -C src clean 

clean+: clean
	rm -f mgconf mgpalloc ; \
	${MAKE} -C doc clean 

install: linux
	sudo cp ./mgpost $(INSTALL_DIR) ; \

