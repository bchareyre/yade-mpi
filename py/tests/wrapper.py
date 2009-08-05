import unittest
from yade.wrapper import *
from miniWm3Wrap import *
from yade._customConverters import *
from math import *

"""
This test module covers python/c++ transitions, for both classes deriving from Serializable,
but also for other classes that we wrap (like Wm3).
"""

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
		pe=ParallelEngine([InsertionSortCollider(),[BoundingVolumeMetaEngine(),BexResetter()]])
		self.assert_(pe.slaves[0].name=='InsertionSortCollider')
		self.assert_(len(pe.slaves[1])==2)
		pe.slaves=[]
		self.assert_(len(pe.slaves)==0)
	##		
	## testing incorrect operations that should raise exceptions
	##
	def testWrongFunctorType(self):
		# dispatchers accept only correct functors
		self.assertRaises(TypeError,lambda: ConstitutiveLawDispatcher([InteractingSphere2AABB()]))
	def testInvalidAttr(self):
		# accessing invalid attributes raises KeyError
		self.assertRaises(KeyError,lambda: Sphere(attributeThatDoesntExist=42))
		self.assertRaises(KeyError,lambda: Sphere()['attributeThatDoesntExist'])
	
class TestWm3Wrapper(unittest.TestCase):
	def assertVQAlmostEqual(self,v1,v2):
		"floating-point comparison of vectors/quaterions"
		self.assertEqual(len(v1),len(v2));
		for i in range(len(v1)): self.assertAlmostEqual(v1[i],v2[i],msg='Component '+str(i)+' of '+str(v1)+' and '+str(v2))
	def testVector2(self):
		v=Vector2(1,2); v2=Vector2(3,4)
		self.assert_(v+v2==Vector2(4,6))
		self.assert_(Vector2.UNIT_X.Dot(Vector2.UNIT_Y)==0)
		self.assert_(Vector2.ZERO.Length()==0)
	def testVector3(self):
		v=Vector3(3,4,5); v2=Vector3(3,4,5)
		self.assert_(v[0]==3 and v[1]==4 and v[2]==5)
		self.assert_(v.SquaredLength()==50)
		self.assert_(v==(3,4,5)) # comparison with list/tuple
		self.assert_(v==[3,4,5])
		self.assert_(v==v2)
		x,y,z,one=Vector3.UNIT_X,Vector3.UNIT_Y,Vector3.UNIT_Z,Vector3.ONE
		self.assert_(x+y+z==one)
		self.assert_(x.Dot(y)==0)
		self.assert_(x.Cross(y)==z)
	def testQuaternion(self):
		# construction
		q1=Quaternion((0,0,1),pi/2)
		q2=Quaternion(Vector3(0,0,1),pi/2)
		q1==q2
		x,y,z,one=Vector3.UNIT_X,Vector3.UNIT_Y,Vector3.UNIT_Z,Vector3.ONE
		self.assertVQAlmostEqual(q1*x,y)
		self.assertVQAlmostEqual(q1*q1*x,-x)
		self.assertVQAlmostEqual(q1*q1.Conjugate(),Quaternion.IDENTITY)
		self.assertVQAlmostEqual(q1.ToAxisAngle()[0],(0,0,1))
		self.assertAlmostEqual(q1.ToAxisAngle()[1],pi/2)
	# not really wm3 thing, but closely related
	def testSe3Conversion(self):
		pp=PhysicalParameters()
		pp['se3']=(Vector3.ZERO,Quaternion.IDENTITY)
		self.assert_(pp['se3'][0]==Vector3.ZERO)
		self.assert_(pp['se3'][1]==Quaternion.IDENTITY)
		pp['se3']=((1,2,3),Quaternion((1,1,1),pi/4))
		self.assert_(pp['se3'][0]==(1,2,3))
		self.assert_(pp['se3'][0]==pp.pos)
		self.assert_(pp['se3'][1]==Quaternion((1,1,1),pi/4))
		self.assert_(pp['se3'][1]==pp.ori)
		
	
