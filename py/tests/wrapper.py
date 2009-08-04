import unittest
from yade.wrapper import *

# copied from PythonUI_rc, should be in some common place (utils? runtime?)
def listChildClassesRecursive(base):
	ret=set(O.childClasses(base)); ret2=set()
	for bb in ret:
		ret2|=listChildClassesRecursive(bb)
	return ret | ret2


rootClasses=set([
	'StandAloneEngine','DeusExMachina','GeometricalModel','InteractingGeometry','PhysicalParameters','BoundingVolume','InteractingGeometry','InteractionPhysics','FileGenerator',
	'BoundingVolumeEngineUnit','GeometricalModelEngineUnit','InteractingGeometryEngineUnit','InteractionGeometryEngineUnit','InteractionPhysicsEngineUnit','PhysicalParametersEngineUnit','PhysicalActionDamperUnit','PhysicalActionApplierUnit','ConstitutiveLaw',
	'BoundingVolumeMetaEngine','GeometricalModelMetaEngine','InteractingGeometryMetaEngine','InteractionGeometryMetaEngine','InteractionPhysicsMetaEngine','PhysicalParametersMetaEngine','PhysicalActionDamper','PhysicalActionApplier','ConstitutiveLawDispatcher'])
allClasses=listChildClassesRecursive('Serializable')

class TestObjectInstantiation(unittest.TestCase):
	def setUp(self):
		pass # no setup needed for tests here
	def testSerializableCtors(self):
		# correct instances created with Serializable('Foo') syntax
		for r in rootClasses:
			obj=Serializable(r); self.assert_(obj.name==r,'Failed for '+r)
	def testRootCtors(self):
		# correct instances created with Foo() syntax
		for r in rootClasses:
			obj=eval(r)(); self.assert_(obj.name==r,'Failed for '+r)
	def testSerializableCtors_attrs_few(self):
		# attributes passed when using the Serializable('Foo',attr1=value1,attr2=value2) syntax
		gm=Serializable('GeometricalModel',wire=True); self.assert_(gm['wire']==True)
	def testRootDerivedCtors(self):
		# classes that are not root classes but derive from them can be instantiated by their name
		for r in rootClasses:
			for c in listChildClassesRecursive(r):
				obj=eval(c)(); self.assert_(obj.name==c,'Failed for '+c)
	def testRootDerivedCtors_attrs_few(self):
		# attributes passed when using the Foo(attr1=value1,attr2=value2) syntax
		gm=GeometricalModel(wire=True); self.assert_(gm['wire']==True)
	def testNonderived_attrs_few(self):
		# classes deriving just from Serializable can be instantiated by their name directly, including attributes
		glds=GLDrawSphere(glutUse=True,glutSlices=24); self.assert_(glds.name=='GLDrawSphere')
	def testDispatcherCtor(self):
		# dispatchers take list of their functors in the ctor
		# same functors are collapsed in one
		cld1=ConstitutiveLawDispatcher([Law2_Dem3Dof_Elastic_Elastic(),Law2_Dem3Dof_Elastic_Elastic()]); self.assert_(len(cld1.functors)==1)
		# two different make two different, right?
		cld2=ConstitutiveLawDispatcher([Law2_Dem3Dof_Elastic_Elastic(),Law2_Dem3DofGeom_CpmPhys_Cpm()]); self.assert_(len(cld2.functors)==2)
	def testInteractionDispatchersCtor(self):
		# InteractionDispatchers takes 3 lists
		id=InteractionDispatchers([ef2_Facet_Sphere_Dem3DofGeom(),ef2_Sphere_Sphere_Dem3DofGeom()],[SimpleElasticRelationships()],[Law2_Dem3Dof_Elastic_Elastic()],)
		self.assert_(len(id.geomDispatcher.functors)==2)
		self.assert_(id.geomDispatcher.name=='InteractionGeometryMetaEngine')
		self.assert_(id.physDispatcher.functors[0].name=='SimpleElasticRelationships')
		self.assert_(id.constLawDispatcher.functors[0].name=='Law2_Dem3Dof_Elastic_Elastic')
	def testParallelEngineCtor(self):
		pass
	## test what shold fail:
	## passing wrong EngineUnit type to a dispatcher
	## reading/writing non-existent attribute

def run():
	suite=unittest.TestLoader().loadTestsFromTestCase(TestObjectInstantiation)
	unittest.TextTestRunner(verbosity=2).run(suite)
	

		
