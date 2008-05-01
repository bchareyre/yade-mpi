# os.path contains $PREFIX/lib/yade$SUFFIX/{extra,gui}
# YADE_CONFIG_DIR=~/.yade$SUFFIX is defined
#

## initialization

from yade import runtime
import sys
sys.excepthook=sys.__excepthook__ # apport on ubuntu override this, we don't need it
sys.path.insert(0,runtime.prefix+'/lib/yade'+runtime.suffix+'/extra')
sys.path.insert(0,runtime.prefix+'/lib/yade'+runtime.suffix+'/gui')

from yade.wrapper import *

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
	
