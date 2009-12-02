# encoding: utf-8
"""Run everything that is needed at the beginning of PythonUI session. 

yade.runtime must have already been populated from within c++."""

import sys
from math import *
from miniWm3Wrap import *
from yade.wrapper import *
from yade._customConverters import *
from yade import runtime
from yade import utils

import yade.sys
yade.sys.setExitHandlers()
yade.sys.injectCtors()
yade.sys.runServers()


if not runtime.__dict__.has_key('noSession'):
	runtime.argv=[runtime.script]+runtime.argv
	sys.argv=runtime.argv # could be [] as well

	try:
		## run simulation if requested from the command line
		if runtime.simulation:
			print "Running simulation "+runtime.simulation
			o=Omega(); o.load(runtime.simulation); o.run();

		## run script if requested from the command line
		if runtime.script:
			print "Running script "+runtime.script
			# an exception from python would propagate to c++ unhandled and cause crash
			try:
				execfile(runtime.script)
			except SystemExit: raise
			except: # all other exceptions
				import traceback
				traceback.print_exc()
				if(runtime.nonInteractive or runtime.stopAfter): sys.exit(1)
		if runtime.stopAfter:
			print "Finished, bye."
			sys.exit(0)

		# run commands if requested from the command line
		#if yadeRunCommands:
		#	print "Running commands from commandline: "+yadeRunCommands
		#	exec(yadeRunCommands)

		if runtime.nonInteractive:
			import time;
			while True: time.sleep(1)
		else:
			sys.argv[0]='<embedded python interpreter>'
			from IPython.Shell import IPShellEmbed
			ipshell = IPShellEmbed(banner=r"""
		__   __    ____          ____                      _      
		\ \ / /_ _|  _ \  ___   / ___|___  _ __  ___  ___ | | ___ 
		 \ V / _` | | | |/ _ \ | |   / _ \| '_ \/ __|/ _ \| |/ _ \ 
		  | | (_| | |_| |  __/ | |__| (_) | | | \__ \ (_) | |  __/
		  |_|\__,_|____/ \___|  \____\___/|_| |_|___/\___/|_|\___|
		""",exit_msg='Bye.'
			,rc_override={'execfile':[runtime.prefix+'/lib/yade'+runtime.suffix+'/py/yade/ipython.py']})

			ipshell()
			# save history -- a workaround for atexit handlers not being run (why?)
			# http://lists.ipython.scipy.org/pipermail/ipython-user/2008-September/005839.html
			import IPython.ipapi
			IPython.ipapi.get().IP.atexit_operations()
	except SystemExit:
		cleanup()
	finally:
		cleanup()

	
