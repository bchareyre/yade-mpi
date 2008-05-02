# encoding: utf-8
"""Run everything that is needed at the beginning of PythonUI session. 

yade.runtime must have already been populated from within c++."""

from yade import runtime
import sys
sys.excepthook=sys.__excepthook__ # apport on ubuntu override this, we don't need it
# sys.path.insert(0,runtime.prefix+'/lib/yade'+runtime.suffix+'/extra')

from yade.wrapper import *

# python2.4 workaround (so that quit() works as it does in 2.5)
if not callable(__builtins__.quit):
	def _quit(): import sys; sys.exit(0)
	__builtins__.quit=_quit

## run simulation if requested from the command line
if runtime.simulation:
	print "Running simulation "+runtime.simulation
	o=Omega(); o.load(runtime.simulation); o.run();

## run script if requested from the command line
if runtime.script:
	print "Running script "+runtime.script
	execfile(runtime.script)
	if runtime.stopAfter: sys.exit(0)

# run commands if requested from the command line
#if yadeRunCommands:
#	print "Running commands from commandline: "+yadeRunCommands
#	exec(yadeRunCommands)

# this cannot be run directly, since importing * is allowed only at module level

try:
	# prefer ipython, since it is colorful and cool
	from IPython.Shell import IPShellEmbed
	sys.argv=['<embedded python interpreter>']
	ipshell = IPShellEmbed()
	ipshell()
except ImportError:
	## no ipython :-(, let's emulate it
	# http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/473900
	import os, readline, rlcompleter, atexit
	history_file = os.path.join(os.environ['HOME']+'/.yade_python_history')
	try:
		 readline.read_history_file(history_file)
	except IOError:
		 pass
	readline.parse_and_bind("tab: complete")
	readline.set_history_length(1000)
	atexit.register(readline.write_history_file, history_file) # FIXME: this will probably not work!
	#del os, readline, rlcompleter, atexit, history_file, __file__

	# run interactive loop
	import code
	code.InteractiveConsole(globals()).interact()

