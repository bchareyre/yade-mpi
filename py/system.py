# coding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>

"""
Functions for accessing yade's internals; only used internally.
"""
import sys
from yade import wrapper
from yade._customConverters import *
from yade import runtime
from yade import config
O=wrapper.Omega()

def childClasses(base,recurse=True,includeBase=False):
	"""Enumerate classes deriving from given base (as string), recursively by default. Returns set."""
	ret=set(O.childClassesNonrecursive(base)); ret2=set();
	if includeBase: ret|=set([base])
	if not recurse: return ret
	for bb in ret:
		ret2|=childClasses(bb)
	return ret | ret2

_allSerializables=childClasses('Serializable')
# classes that cannot be instantiated in python directly, and will have no properties generated for them
_noPropsClasses=set(['InteractionContainer','BodyContainer','Functor','Engine','Dispatcher'])
# classes that have special wrappers; only the most-bottom ones, with their names as it is in c++
_pyRootClasses=set([
	'GlobalEngine','PartialEngine','Shape','Bound','InteractionGeometry','InteractionPhysics','FileGenerator',
	'BoundFunctor','InteractionGeometryFunctor','InteractionPhysicsFunctor','LawFunctor','Material','State']
	# childless classes
	+['BoundDispatcher','InteractionGeometryDispatcher','InteractionPhysicsDispatcher','LawDispatcher','InteractionDispatchers','ParallelEngine']
)
## set of classes for which the proxies were created
_proxiedClasses=set()

## deprecated classes
# if old class name is used, the new object is constructed and a warning is issued about old name being used
# keep chronologically ordered, oldest first; script/rename-class.py appends at the end
_deprecated={
	'CpmPhysDamageColorizer':'CpmStateUpdater', # renamed 10.10.2009
	'GLDraw_Dem3DofGeom_FacetSphere':'Gl1_Dem3DofGeom_FacetSphere', # renamed 15.11.2009
	'PeriodicInsertionSortCollider':'InsertionSortCollider',	# integrated 25.11.2009
	'BoundingVolumeMetaEngine':'BoundDispatcher', # Tue Dec  1 14:28:29 2009, vaclav@flux  ## was BoundingVolumeDispatcher, generating double warning
	'BoundingVolumeEngineUnit':'BoundFunctor', # Tue Dec  1 14:39:53 2009, vaclav@flux ## was BoundingVolumeFunctor, generating double warning
	'InteractionGeometryMetaEngine':'InteractionGeometryDispatcher', # Tue Dec  1 14:40:36 2009, vaclav@flux
	'InteractionPhysicsMetaEngine':'InteractionPhysicsDispatcher', # Tue Dec  1 14:40:53 2009, vaclav@flux
	'InteractionPhysicsEngineUnit':'InteractionPhysicsFunctor', # Tue Dec  1 14:41:19 2009, vaclav@flux
	'InteractionGeometryEngineUnit':'InteractionGeometryFunctor', # Tue Dec  1 14:41:56 2009, vaclav@flux
	'BoundingVolume':'Bound', # Fri Dec  4 23:44:14 2009, vaclav@flux
	'BoundingVolumeDispatcher':'BoundDispatcher', # Fri Dec  4 23:44:27 2009, vaclav@flux
	'BoundingVolumeFunctor':'BoundFunctor', # Fri Dec  4 23:44:34 2009, vaclav@flux
	'InteractingGeometry':'Shape', # Fri Dec  4 23:48:13 2009, vaclav@flux
	'ConstitutiveLaw':'LawFunctor', # Fri Dec  4 23:57:51 2009, vaclav@flux
	'ConstitutiveLawDispatcher':'LawDispatcher', # Fri Dec  4 23:57:59 2009, vaclav@flux
	'ef2_Sphere_Sphere_Dem3DofGeom':'Ig2_Sphere_Sphere_Dem3DofGeom', # Sat Dec  5 00:02:03 2009, vaclav@flux
	'ef2_Facet_Sphere_Dem3DofGeom':'Ig2_Facet_Sphere_Dem3DofGeom', # Sat Dec  5 00:02:13 2009, vaclav@flux
	'ef2_Wall_Sphere_Dem3DofGeom':'Ig2_Wall_Sphere_Dem3DofGeom', # Sat Dec  5 00:02:22 2009, vaclav@flux
	'NewtonsDampedLaw':'NewtonIntegrator', # Wed Dec  9 17:44:15 2009, vaclav@flux
	'StandAloneEngine':'GlobalEngine', # Wed Dec  9 17:46:12 2009, vaclav@flux
	'DeusExMachina':'PartialEngine', # Wed Dec  9 17:53:06 2009, vaclav@flux
	## 'Sphere':'SphereModel', # 11/12/2009, vaclav@flux   ### comment out to prevent loop, since InteractingSphere was renamed to Sphere later
	## 'Facet':'FacetModel', # Fri Dec 11 15:27:41 2009, vaclav@flux   ### dtto
	## 'Box':'BoxModel', # Fri Dec 11 15:39:44 2009, vaclav@flux       ### dtto
	'GLDrawSphere':'GLDrawSphereModel', # Fri Dec 11 15:43:31 2009, vaclav@flux
	'GLDrawBox':'GLDrawBoxModel', # Fri Dec 11 15:43:50 2009, vaclav@flux
	'GLDrawFacet':'GLDrawFacetModel', # Fri Dec 11 15:44:04 2009, vaclav@flux
	'GLDrawInteractingGeometryFunctor':'GlShapeFunctor', # Fri Dec 11 16:08:16 2009, vaclav@flux
	'GLDrawGeometricalModelFunctor':'GlGeometricalModelFunctor', # Fri Dec 11 16:08:40 2009, vaclav@flux
	'GLDrawBoundingVolumeFunctor':'GlBoundFunctor', # Fri Dec 11 16:09:16 2009, vaclav@flux
	'GLDrawInteractionGeometryFunctor':'GlInteractionGeometryFunctor', # Fri Dec 11 16:09:48 2009, vaclav@flux
	'GLDrawInteractionPhysicsFunctor':'GlInteractionPhysicsFunctor', # Fri Dec 11 16:10:02 2009, vaclav@flux
	'GLDrawStateFunctor':'GlStateFunctor', # Fri Dec 11 16:10:29 2009, vaclav@flux
	'GLDrawInteractingSphere':'Gl1_Sphere', # Fri Dec 11 16:20:51 2009, vaclav@flux
	'GLDrawInteractingFacet':'Gl1_Facet', # Fri Dec 11 16:21:06 2009, vaclav@flux
	'GLDrawInteractingBox':'Gl1_Box', # Fri Dec 11 16:21:17 2009, vaclav@flux
	'TetraDraw':'Gl1_Tetra', # Fri Dec 11 16:22:18 2009, vaclav@flux
	'MetaInteractingGeometry':'SceneShape', # Fri Dec 11 16:56:06 2009, vaclav@flux
	'GLDrawMetaInteractingGeometry':'Gl1_SceneShape', # Fri Dec 11 17:00:00 2009, vaclav@flux
	'GLDrawMetaInteractingGeometry':'Gl1_SceneShape', # Fri Dec 11 17:10:17 2009, vaclav@flux
	'SpheresContactGeometry':'ScGeom', # Sun Dec 13 20:59:09 2009, vaclav@flux
	'InteractingSphere2InteractingSphere4SpheresContactGeometry':'Ig2_Sphere_Sphere_ScGeom', # Sun Dec 13 21:02:31 2009, vaclav@flux
	'InteractingFacet2InteractingSphere4SpheresContactGeometry':'Ig2_Facet_Sphere_ScGeom', # Sun Dec 13 21:02:57 2009, vaclav@flux
	'InteractingBox2InteractingSphere4SpheresContactGeometry':'Ig2_Box_Sphere_ScGeom', # Sun Dec 13 21:03:10 2009, vaclav@flux
	'InteractingSphere':'Sphere', # Sun Dec 13 21:13:40 2009, vaclav@flux
	'InteractingFacet':'Facet', # Sun Dec 13 21:15:07 2009, vaclav@flux
	'InteractingBox':'Box', # Sun Dec 13 21:15:16 2009, vaclav@flux
	'MetaInteractingGeometry2AABB':'DontUseClass_MetaInteractingGeometry2AABB__youCanRemoveItSafelyFromYourSimulation_itWillBeRemovedInTheFutureCompletely', # Mon Dec 14 09:33:20 2009, vaclav@flux
	'InteractingSphere2AABB':'Bo1_Sphere_Aabb', # Thu Dec 17 08:59:29 2009, vaclav@falx
	'InteractingFacet2AABB':'Bo1_Facet_Aabb', # Thu Dec 17 08:59:56 2009, vaclav@falx
	'InteractingBox2AABB':'Bo1_Box_Aabb', # Thu Dec 17 09:00:14 2009, vaclav@falx
	'Wall2AABB':'Bo1_Wall_Aabb', # Thu Dec 17 09:01:07 2009, vaclav@falx
	'AABB':'Aabb', # Thu Dec 17 09:01:35 2009, vaclav@falx
	'GLDrawAABB':'Gl1_Aabb', # Thu Dec 17 09:07:58 2009, vaclav@falx
	'GLDraw_Dem3DofGeom_SphereSphere':'Gl1_Dem3DofGeom_SphereSphere', # Fri Dec 18 23:01:39 2009, vaclav@flux
	'BexContainer':'ForceContainer', # Fri Dec 25 11:43:44 2009, vaclav@flux
	'BexResetter':'ForceResetter', # Fri Dec 25 11:44:01 2009, vaclav@flux
	'PhysicalActionContainerResetter':'ForceResetter', # Fri Dec 25 11:45:32 2009, vaclav@flux
	'PhysicalActionContainerReseter':'ForceResetter', # Fri Dec 25 11:45:51 2009, vaclav@flux
	'GranularMat':'FrictMat', # Sun Jan 10 09:26:45 2010, vaclav@flux
	'SimpleElasticRelationships':'Ip2_FrictMat_FrictMat_NormShearPhys', # Sun Jan 10 09:28:17 2010, vaclav@flux
	'NormalInteraction':'NormPhys', # Sun Jan 10 09:28:56 2010, vaclav@flux
	'NormalShearInteraction':'NormShearPhys', # Sun Jan 10 09:29:22 2010, vaclav@flux
	'ElasticMat':'ElastMat', # Sun Jan 10 09:53:15 2010, vaclav@flux
	'ElasticContactInteraction':'FrictPhys', # Sun Jan 10 09:57:59 2010, vaclav@flux
	'ef2_Spheres_Elastic_ElasticLaw':'Law2_ScGeom_FrictPhys_Basic', # Sun Jan 10 09:59:42 2010, vaclav@flux
	'Law2_Dem3Dof_Elastic_Elastic':'Law2_Dem3Dof_FrictPhys_Basic', # Sun Jan 10 10:00:25 2010, vaclav@flux
	'Law2_Dem3Dof_FrictPhys_Basic':'Law2_Dem3DofGeom_FrictPhys_Basic', # Sun Jan 10 10:01:27 2010, vaclav@flux
	'Ip2_FrictMat_FrictMat_NormShearPhys':'Ip2_FrictMat_FrictMat_FrictPhys', # Sun Jan 10 10:07:40 2010, vaclav@flux
	'ContactLaw1':'RockJointLaw', # Tue Feb  2 14:53:49 2010, jduriez@c1solimara-l
	'ContactLaw1':'RockJointLaw', # Tue Feb  2 14:54:40 2010, jduriez@c1solimara-l
	'ContactLaw1Interaction':'RockJointPhys', # Tue Feb  2 14:55:34 2010, jduriez@c1solimara-l
	'CL1Relationships':'RockJointLawRelationships', # Tue Feb  2 14:58:02 2010, jduriez@c1solimara-l
	'GLDrawCpmPhys':'Gl1_CpmPhys', # Sat Feb  6 14:46:08 2010, vaclav@flux
	'RockJointLaw':'NormalInelasticityLaw', # Mon Feb  8 11:17:00 2010, jduriez@c1solimara-l
	'RockJointPhys':'NormalInelasticityPhys', # Mon Feb  8 11:17:35 2010, jduriez@c1solimara-l
	'RockJointLawRelationships':'Ip2_2xCohFrictMat_NormalInelasticityPhys', # Mon Feb  8 11:17:59 2010, jduriez@c1solimara-l
	'TetraBang':'TTetraGeom', # Tue Feb  9 10:21:24 2010, vaclav@flux
	'TetraMold':'Tetra', # Tue Feb  9 10:22:15 2010, vaclav@flux
	'TetraAABB':'Bo1_Tetra_Aabb', # Tue Feb  9 10:22:33 2010, vaclav@flux
	'Tetra2TetraBang':'Ig2_Tetra_Tetra_TTetraGeom', # Tue Feb  9 10:23:19 2010, vaclav@flux
	'TetraLaw':'TetraVolumetricLaw', # Tue Feb  9 10:24:10 2010, vaclav@flux
	'DirecResearchEngine':'Disp2DPropLoadEngine', # Wed Mar 10 12:23:42 2010, jduriez@c1solimara-l
	'CinemCNCEngine':'KinemCNLEngine', # Wed Mar 10 12:33:36 2010, jduriez@c1solimara-l
	'CinemKNCEngine':'KinemCNSEngine', # Wed Mar 10 12:34:01 2010, jduriez@c1solimara-l
	'CinemDNCEngine':'KinemCNDEngine', # Wed Mar 10 12:34:27 2010, jduriez@c1solimara-l
	'CinemDTCEngine':'KinemCTDEngine', # Wed Mar 10 12:34:37 2010, jduriez@c1solimara-l
	'Ip2_BMP_BMP_CSPhys':'Ip2_2xFrictMat_CSPhys', # Wed Mar 10 15:08:56 2010, eudoxos@frigo
	'CinemDTCEngine':'KinemCTDEngine', # Tue Mar 16 13:54:21 2010, jduriez@c1solimara-l
	'NormalInelasticityLaw':'Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity', # Wed Mar 17 15:50:59 2010, jduriez@c1solimara-l
	'OldName':'NewName', # Tue Mar 30 14:11:13 2010, sch50p@fluent-ph
	'CapillaryCohesiveLaw':'CapillaryLaw', # Tue Mar 30 14:11:36 2010, sch50p@fluent-ph
	'Simplecd':'/home/sch50p/YADE-bzr/yade/pkg/dem/PreProcessor', # Tue Mar 30 14:13:03 2010, sch50p@fluent-ph
	'SimpleElasticRelationshipsWater':'Ip2_Frictmat_FrictMat_CapillaryLawPhys', # Tue Mar 30 14:20:36 2010, sch50p@fluent-ph
	'OldName':'NewName', # Wed Mar 31 09:22:48 2010, sch50p@fluent-ph
	'CapillaryLaw':'Law2_ScGeom_CapillaryPhys_Capillarity', # Wed Mar 31 09:23:36 2010, sch50p@fluent-ph
	'CapillaryParameters':'CapillaryPhys', # Wed Mar 31 09:25:03 2010, sch50p@fluent-ph
	'Ip2_FrictMat_FrictMat_CapillaryLawPhys':'Ip2_FrictMat_FrictMat_CapillaryPhys', # Wed Mar 31 09:26:04 2010, sch50p@fluent-ph
	'Ip2_Frictmat_FrictMat_CapillaryLawPhys':'Ip2_FrictMat_FrictMat_CapillaryPhys', # Wed Mar 31 09:26:56 2010, sch50p@fluent-ph
	'SimpleViscoelasticMat':'ViscElMat', # Fri Apr  9 19:25:38 2010, vaclav@flux
	'SimpleViscoelasticPhys':'ViscElPhys', # Fri Apr  9 19:26:34 2010, vaclav@flux
	'Law2_Spheres_Viscoelastic_SimpleViscoelastic':'Law2_ScGeom_ViscElPhys_Basic', # Fri Apr  9 19:28:02 2010, vaclav@flux
	'Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys':'Ip2_ViscElMat_ViscElMat_ViscElPhys', # Fri Apr  9 19:28:48 2010, vaclav@flux
	'MomentEngine':'TorqueEngine', # Sun May  2 16:09:34 2010, vaclav@flux
	'JumpChangeSe3':'StepDisplacer', # Sun May  2 16:14:21 2010, vaclav@flux
	'ContactLaw1':'SomeBetterName', # Fri Jun  4 15:35:38 2010, jduriez@c1solimara-l
	'Ip2_2xCohFrictMat_NormalInelasticityPhys':'Ip2_2xNormalInelasticMat_NormalInelasticityPhys', # Fri Jun  4 15:36:41 2010, jduriez@c1solimara-l
	'Ip2_2xCohFrictMat_NormalInelasticityPhys':'Ip2_2xNormalInelasticMat_NormalInelasticityPhys', # Fri Jun  4 15:37:01 2010, jduriez@c1solimara-l
	'Ip2_2xCohFrictMat_NormalInelasticityPhys':'Ip2_2xNormalInelasticMat_NormalInelasticityPhys', # Fri Jun  4 15:37:16 2010, jduriez@c1solimara-l
	'OpenGLRenderingEngine':'OpenGLRenderer', # Sat Jul 24 06:04:13 2010, vaclav@flux
	'PeriodicPythonRunner':'PyRunner', # Wed Sep  1 16:41:50 2010, chia@engs-018373
	### END_RENAMED_CLASSES_LIST ### (do not delete this line; scripts/rename-class.py uses it
}

def updateScripts(scripts):
	## Thanks goes to http://code.activestate.com/recipes/81330-single-pass-multiple-replace/
	from UserDict import UserDict
	import re,os
	class Xlator(UserDict):
		"An all-in-one multiple string substitution class; adapted to match only whole words"
		def _make_regex(self): 
			"Build a regular expression object based on the keys of the current dictionary"
			return re.compile(r"(\b%s\b)" % "|".join(self.keys()))  ## adapted here 
		def __call__(self, mo): 
			"This handler will be invoked for each regex match"
			# Count substitutions
			self.count += 1 # Look-up string
			return self[mo.string[mo.start():mo.end()]]
		def xlat(self, text):
			"Translate text, returns the modified text."
			# Reset substitution counter
			self.count = 0 
			# Process text
			return self._make_regex().sub(self, text)
	# use the _deprecated dictionary for translation, but only when matching on words boundary
	xlator=Xlator(_deprecated)
	if len(scripts)==0: print "No scripts given to --update. Nothing to do."
	for s in scripts:
		if not s.endswith('.py'): raise RuntimeError("Refusing to do --update on file '"+s+"' (not *.py)")
		txt=open(s).read()
		txt2=xlator.xlat(txt)
		if xlator.count==0: print "%s: already up-to-date."%s
		else:
			os.rename(s,s+'~')
			out=open(s,'w'); out.write(txt2); out.close()
			print "%s: %d subtitution%s made, backup in %s~"%(s,xlator.count,'s' if xlator.count>1 else '',s)
				

def cxxCtorsDict(proxyNamespace=__builtins__):
	"""Return dictionary of class constructors for yade's c++ types, which should be used to update a namespace.
	
	Root classes are those that are directly wrapped by boost::python. These are only put to the dict.

	Derived classes (from these root classes) are faked by creating a callable which invokes appropriate root class constructor with the derived class parameter and passes remaining arguments to it.

	Classes that are neither root nor derived are exposed via callable object that constructs a Serializable of given type and passes the parameters.
	"""
	proxyNamespace={}

	import yade.wrapper
	for c in _allSerializables:
		try:
			proxyNamespace[c]=yade.wrapper.__dict__[c]
		except KeyError: pass # not registered properly

	# added 2/9/2010, should be removed before christmas 2010!
	def Serializable_name(obj):
		nm=obj.__class__.__name__
		print 'WARN: %s.name is deprecated, use:\nWARN: * %s.__class__.__name__ to get the class name (as string)\nWARN: * isinstance(object,%s) to test whether object is of type %s.\n'%(nm,nm,nm,nm)
		return nm
	yade.wrapper.Serializable.name=property(Serializable_name)

	# deprecated names
	for oldName in _deprecated.keys():
		class warnWrap:
			def __init__(self,_old,_new):
				# assert(proxyNamespace.has_key(_new))
				self.old,self.new=_old,_new
			def __call__(self,*args,**kw):
				import warnings; warnings.warn("Class `%s' was renamed to (or replaced by) `%s', update your code! (you can run 'yade --update script.py' to do that automatically)"%(self.old,self.new),DeprecationWarning,stacklevel=2);
				return yade.wrapper.__dict__[self.new](*args,**kw)
		proxyNamespace[oldName]=warnWrap(oldName,_deprecated[oldName])
	return proxyNamespace


def setExitHandlers():
	"""Set exit handler to avoid gdb run if log4cxx crashes at exit."""
	# avoid backtrace at regular exit, even if we crash
	if 'log4cxx' in config.features:
		__builtins__['quit']=wrapper.Omega().exitNoBacktrace
		sys.exit=wrapper.Omega().exitNoBacktrace
	# this seems to be not needed anymore:
	#sys.excepthook=sys.__excepthook__ # apport on ubuntu overrides this, we don't need it


# consistency check
# if there are no serializables, then plugins were not loaded yet, probably
if(len(_allSerializables)==0):
	raise ImportError("No classes deriving from Serializable found; you must call yade.boot.initialize to load plugins before importing yade.system.")

