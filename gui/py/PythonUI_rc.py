# encoding: utf-8
"""Run everything that is needed at the beginning of PythonUI session. 

yade.runtime must have already been populated from within c++."""

import sys
sys.excepthook=sys.__excepthook__ # apport on ubuntu overrides this, we don't need it
# sys.path.insert(0,runtime.prefix+'/lib/yade'+runtime.suffix+'/extra')

from math import *
from miniWm3Wrap import *
from yade.wrapper import *
from yade._customConverters import *
from yade import runtime
from yade import utils
__builtins__.O=Omega()

### direct object creation through automatic wrapper functions
def listChildClassesRecursive(base):
	ret=set(O.childClasses(base)); ret2=set()
	for bb in ret:
		ret2|=listChildClassesRecursive(bb)
	return ret | ret2
# not sure whether __builtins__ is the right place?
_dd=__builtins__.__dict__
_allClasses=set(listChildClassesRecursive('Serializable'))
_proxiedClasses=set()
_classTranslations={'FileGenerator':'Preprocessor'}

for root in [
	'StandAloneEngine','DeusExMachina','GeometricalModel','InteractingGeometry','PhysicalParameters','BoundingVolume','InteractingGeometry','InteractionPhysics','FileGenerator',
		# functors
		'BoundingVolumeEngineUnit','GeometricalModelEngineUnit','InteractingGeometryEngineUnit','InteractionGeometryEngineUnit','InteractionPhysicsEngineUnit','PhysicalParametersEngineUnit','PhysicalActionDamperUnit','PhysicalActionApplierUnit','ConstitutiveLaw'
	]:
	root2=root if (root not in _classTranslations.keys()) else _classTranslations[root]
	for p in listChildClassesRecursive(root):
		#class argStorage:
		#	def __init__(self,_root,_class): self._root,self._class=_root,_class
		#	def __call__(self,*args): return yade.wrapper.__dict__[self._root](self._class,*args)
		#if root=='MetaEngine': _dd[p]=argStorage(root2,p)
		_dd[p]=lambda __r_=root2,__p_=p,**kw : yade.wrapper.__dict__[__r_](__p_,**kw) # eval(root2)(p,kw)
		_proxiedClasses.add(p)
#_dd['Generic']=yade.wrapper.Serializable
for p in _allClasses-_proxiedClasses: # wrap classes that don't derive from any specific root class, just from Serializable
	_dd[p]=lambda __p_=p,**kw: yade.wrapper.Serializable(__p_,**kw)
### end wrappers

#### HANDLE RENAMED CLASSES ####
# if old class name is used, the new object is constructed and a warning is issued about old name being used
renamed={
	# renamed 23.5.2009, may be removed in a few months
	'BrefcomMakeContact':'Ip2_CpmMat_CpmMat_CpmPhys',
	'BrefcomContact':'CpmPhys',
	'BrefcomPhysParams':'CpmMat',
	'ef2_Spheres_Brefcom_BrefcomLaw':'Law2_Dem3DofGeom_CpmPhys_Cpm',
	'GLDrawBrefcomContact':'GLDrawCpmPhys',
	'BrefcomDamageColorizer':'CpmPhysDamageColorizer',
	'BrefcomGlobalCharacteristics':'CpmGlobalCharacteristics',
}

for oldName in renamed:
	class warnWrap:
		def __init__(self,_old,_new): self.old,self.new=_old,_new
		def __call__(self,*args,**kw):
			import warnings; warnings.warn("Class `%s' was renamed to `%s', update your code!"%(self.old,self.new),DeprecationWarning,stacklevel=2);
			return _dd[self.new](*args,**kw)
	_dd[oldName]=warnWrap(oldName,renamed[oldName])



# python2.4 workaround (so that quit() works as it does in 2.5)
if not callable(__builtins__.quit):
	def _quit(): import sys; sys.exit(0)
	__builtins__.quit=_quit

def cleanup():
	try:
		#import yade.qt, time
		import yade._qt
		if yade._qt.isActive(): yade._qt.close();
		#yade.qt.close()
		#while True: time.sleep(.1) # wait to be killed
	except ImportError: pass


## run the TCP server
import yade.PythonTCPServer
srv=yade.PythonTCPServer.PythonTCPServer(minPort=9000)
yade.runtime.cookie=srv.server.cookie
sys.stdout.flush()
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

	
