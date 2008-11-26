# encoding: utf-8
"""Run everything that is needed at the beginning of PythonUI session. 

yade.runtime must have already been populated from within c++."""

import sys
sys.excepthook=sys.__excepthook__ # apport on ubuntu overrides this, we don't need it
# sys.path.insert(0,runtime.prefix+'/lib/yade'+runtime.suffix+'/extra')

from yade.wrapper import *
from yade import runtime
from yade import utils
__builtins__.O=Omega()

#try:
#	import yade.qt.atexit
#	atexit.register(yade.qt.close)
#except ImportError: pass

# python2.4 workaround (so that quit() works as it does in 2.5)
if not callable(__builtins__.quit):
	def _quit(): import sys; sys.exit(0)
	__builtins__.quit=_quit

## run the TCP server
import yade.PythonTCPServer
srv=yade.PythonTCPServer.PythonTCPServer(minPort=9000)
yade.runtime.cookie=srv.server.cookie
sys.stdout.flush()

## run simulation if requested from the command line
if runtime.simulation:
	print "Running simulation "+runtime.simulation
	o=Omega(); o.load(runtime.simulation); o.run();

## run script if requested from the command line
if runtime.script:
	print "Running script "+runtime.script
	# an exception from python would propagate to c++ unhandled and cause crash
	try: execfile(runtime.script)
	except SystemExit: raise # re-raise sys.exit
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
	sys.argv=runtime.args # could be [] as well
	from IPython.Shell import IPShellEmbed
	sys.argv=['<embedded python interpreter>']
	ipshell = IPShellEmbed(banner=r"""__   __    ____          ____                      _      
\ \ / /_ _|  _ \  ___   / ___|___  _ __  ___  ___ | | ___ 
 \ V / _` | | | |/ _ \ | |   / _ \| '_ \/ __|/ _ \| |/ _ \ 
  | | (_| | |_| |  __/ | |__| (_) | | | \__ \ (_) | |  __/
  |_|\__,_|____/ \___|  \____\___/|_| |_|___/\___/|_|\___|
""",exit_msg='Bye.'
	,rc_override={'execfile':[runtime.prefix+'/lib/yade'+runtime.suffix+'/gui/yade/ipython.py']})

	ipshell()
	# save history -- a workaround for atexit handlers not being run (why?)
	# http://lists.ipython.scipy.org/pipermail/ipython-user/2008-September/005839.html
	import IPython.ipapi
	IPython.ipapi.get().IP.atexit_operations()
	try:
		import yade.qt
		yade.qt.close()
	except ImportError: pass
