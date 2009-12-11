# encoding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>

"""
This test module covers python/c++ transitions, for both classes deriving from Serializable,
but also for other classes that we wrap (like Wm3).
"""

import unittest
from yade.wrapper import *
from miniWm3Wrap import *
from yade._customConverters import *
from math import *
from yade import system
from yade import *


rootClasses=set(['GlobalEngine','PartialEngine','Shape','Bound','InteractionGeometry','InteractionPhysics','FileGenerator','BoundFunctor','InteractionGeometryFunctor','InteractionPhysicsFunctor','LawFunctor','Material','State'])

allClasses=system.childClasses('Serializable')

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
		gm=Serializable('Shape',wire=True); self.assert_(gm['wire']==True)
	def testRootDerivedCtors(self):
		# classes that are not root classes but derive from them can be instantiated by their name
		for r in rootClasses:
			for c in system.childClasses(r):
				obj=eval(c)(); self.assert_(obj.name==c,'Failed for '+c)
	def testRootDerivedCtors_attrs_few(self):
		# attributes passed when using the Foo(attr1=value1,attr2=value2) syntax
		gm=Shape(wire=True); self.assert_(gm['wire']==True)
	# not applicable for OpenGL-less builds... seems all other classes do derive from something below Serializable
	#def testNonderived_attrs_few(self):
	#	# classes deriving just from Serializable can be instantiated by their name directly, including attributes
	#	gld3d=Gl1_Sphere(glutSlices=24); self.assert_(glds.name=='Gl1_Sphere')
	def testDispatcherCtor(self):
		# dispatchers take list of their functors in the ctor
		# same functors are collapsed in one
		cld1=LawDispatcher([Law2_Dem3Dof_Elastic_Elastic(),Law2_Dem3Dof_Elastic_Elastic()]); self.assert_(len(cld1.functors)==1)
		# two different make two different, right?
		cld2=LawDispatcher([Law2_Dem3Dof_Elastic_Elastic(),Law2_Dem3DofGeom_CpmPhys_Cpm()]); self.assert_(len(cld2.functors)==2)
	def testInteractionDispatchersCtor(self):
		# InteractionDispatchers takes 3 lists
		id=InteractionDispatchers([Ig2_Facet_Sphere_Dem3DofGeom(),Ig2_Sphere_Sphere_Dem3DofGeom()],[SimpleElasticRelationships()],[Law2_Dem3Dof_Elastic_Elastic()],)
		self.assert_(len(id.geomDispatcher.functors)==2)
		self.assert_(id.geomDispatcher.name=='InteractionGeometryDispatcher')
		self.assert_(id.physDispatcher.functors[0].name=='SimpleElasticRelationships')
		self.assert_(id.lawDispatcher.functors[0].name=='Law2_Dem3Dof_Elastic_Elastic')
	def testParallelEngineCtor(self):
		pe=ParallelEngine([InsertionSortCollider(),[BoundDispatcher(),BexResetter()]])
		self.assert_(pe.slaves[0].name=='InsertionSortCollider')
		self.assert_(len(pe.slaves[1])==2)
		pe.slaves=[]
		self.assert_(len(pe.slaves)==0)
	##		
	## testing incorrect operations that should raise exceptions
	##
	def testWrongFunctorType(self):
		# dispatchers accept only correct functors
		self.assertRaises(TypeError,lambda: LawDispatcher([InteractingSphere2AABB()]))
	def testInvalidAttr(self):
		# accessing invalid attributes raises KeyError
		self.assertRaises(KeyError,lambda: InteractingSphere(attributeThatDoesntExist=42))
		self.assertRaises(KeyError,lambda: InteractingSphere()['attributeThatDoesntExist'])
	
class TestWm3Wrapper(unittest.TestCase):
	def assertVQAlmostEqual(self,v1,v2):
		"floating-point comparison of vectors/quaterions"
		self.assertEqual(len(v1),len(v2));
		for i in range(len(v1)): self.assertAlmostEqual(v1[i],v2[i],msg='Component '+str(i)+' of '+str(v1)+' and '+str(v2))
	def testVector2(self):
		v=Vector2(1,2); v2=Vector2(3,4)
		self.assert_(v+v2==Vector2(4,6))
		self.assert_(Vector2().UNIT_X.Dot(Vector2().UNIT_Y)==0)
		self.assert_(Vector2().ZERO.Length()==0)
	def testVector3(self):
		v=Vector3(3,4,5); v2=Vector3(3,4,5)
		self.assert_(v[0]==3 and v[1]==4 and v[2]==5)
		self.assert_(v.SquaredLength()==50)
		self.assert_(v==(3,4,5)) # comparison with list/tuple
		self.assert_(v==[3,4,5])
		self.assert_(v==v2)
		x,y,z,one=Vector3().UNIT_X,Vector3().UNIT_Y,Vector3().UNIT_Z,Vector3().ONE
		self.assert_(x+y+z==one)
		self.assert_(x.Dot(y)==0)
		self.assert_(x.Cross(y)==z)
	def testQuaternion(self):
		# construction
		q1=Quaternion((0,0,1),pi/2)
		q2=Quaternion(Vector3(0,0,1),pi/2)
		q1==q2
		x,y,z,one=Vector3().UNIT_X,Vector3().UNIT_Y,Vector3().UNIT_Z,Vector3().ONE
		self.assertVQAlmostEqual(q1*x,y)
		self.assertVQAlmostEqual(q1*q1*x,-x)
		self.assertVQAlmostEqual(q1*q1.Conjugate(),Quaternion().IDENTITY)
		self.assertVQAlmostEqual(q1.ToAxisAngle()[0],(0,0,1))
		self.assertAlmostEqual(q1.ToAxisAngle()[1],pi/2)
	# not really wm3 thing, but closely related
	# no way to test this currently, as State::se3 is not serialized (State::pos and State::ori are serialized instead...)
	# remove the '_' from the method name to re-enable
	def _testSe3Conversion(self):
		return
		pp=State()
		pp['se3']=(Vector3().ZERO,Quaternion().IDENTITY)
		self.assert_(pp['se3'][0]==Vector3().ZERO)
		self.assert_(pp['se3'][1]==Quaternion().IDENTITY)
		pp['se3']=((1,2,3),Quaternion((1,1,1),pi/4))
		self.assert_(pp['se3'][0]==(1,2,3))
		self.assert_(pp['se3'][0]==pp.pos)
		self.assert_(pp['se3'][1]==Quaternion((1,1,1),pi/4))
		self.assert_(pp['se3'][1]==pp.ori)
		
	
