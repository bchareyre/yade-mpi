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
sys.exit=O.exitNoBacktrace

### direct object creation through automatic wrapper functions
def listChildClassesRecursive(base):
	ret=set(O.childClasses(base)); ret2=set()
	for bb in ret:
		ret2|=listChildClassesRecursive(bb)
	return ret | ret2

# not sure whether __builtins__ is the right place?
_proxyNamespace=__builtins__.__dict__
# all classes that we want to handle at this point
_allSerializables=set(listChildClassesRecursive('Serializable'))
# classes that cannot be instantiated in python directly, and will have no properties generated for them
_noPropsClasses=set(['InteractionContainer','BodyContainer','EngineUnit','Engine','MetaEngine'])
# classes that have special wrappers; only the most-bottom ones, with their names as it is in c++
_pyRootClasses=set([
	'StandAloneEngine','DeusExMachina','GeometricalModel','InteractingGeometry','PhysicalParameters','BoundingVolume','InteractingGeometry','InteractionPhysics','FileGenerator',
	'BoundingVolumeEngineUnit','GeometricalModelEngineUnit','InteractingGeometryEngineUnit','InteractionGeometryEngineUnit','InteractionPhysicsEngineUnit','PhysicalParametersEngineUnit','PhysicalActionDamperUnit','PhysicalActionApplierUnit','ConstitutiveLaw'])
# classes for which proxies were already created
_proxiedClasses=set()


if 1:
	# create types for all classes that yade defines: first those deriving from some root classes
	for root in _pyRootClasses:
		rootType=yade.wrapper.__dict__[root]
		for p in listChildClassesRecursive(root):
			_proxyNamespace[p]=type(p,(rootType,),{'__init__': lambda self,__subType_=p,*args,**kw: super(type(self),self).__init__(__subType_,*args,**kw)})
			_proxiedClasses.add(p)
	# create types for classes that derive just from Serializable
	for p in _allSerializables-_proxiedClasses-_pyRootClasses:
		_proxyNamespace[p]=type(p,(Serializable,),{'__init__': lambda self,__subType_=p,*args,**kw: super(type(self),self).__init__(__subType_,*args,**kw)})
	## NOTE: this will not work if the object is returned from c++; seems to be more confusing than doing any good; disabling for now.
	if 0:
		# create class properties for yade serializable attributes, i.e. access object['attribute'] as object.attribute
		for c in _allSerializables-_noPropsClasses:
			cls=eval(c) # ugly: create instance; better lookup some namespace (builtins? globals?)
			for k in cls().keys(): # must be instantiated so that attributes can be retrieved
				setattr(cls,k,property(lambda self,__k_=k:self.__getitem__(__k_),lambda self,val,__k_=k:self.__setitem__(__k_,val)))
else:
	# old code, can be removed at some point
	for root in _pyRootClasses:
		for p in listChildClassesRecursive(root):
			_proxyNamespace[p]=lambda __r_=root,__p_=p,**kw : yade.wrapper.__dict__[__r_](__p_,**kw)
			_proxiedClasses.add(p)
	# wrap classes that don't derive from any specific root class, have no proxy and need it
	for p in _allSerializables-_proxiedClasses-_pyRootClasses:
		_proxyNamespace[p]=lambda __p_=p,**kw: yade.wrapper.Serializable(__p_,**kw)
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
	# renamed back to comply with the c++ name, 4.8.2009
	'Preprocessor':'FileGenerator',
	'PersistentSAPCollider':'InsertionSortCollider',
}

for oldName in renamed:
	class warnWrap:
		def __init__(self,_old,_new): self.old,self.new=_old,_new
		def __call__(self,*args,**kw):
			import warnings; warnings.warn("Class `%s' was renamed to (or replaced by) `%s', update your code!"%(self.old,self.new),DeprecationWarning,stacklevel=3);
			return _proxyNamespace[self.new](*args,**kw)
	_proxyNamespace[oldName]=warnWrap(oldName,renamed[oldName])



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

	
