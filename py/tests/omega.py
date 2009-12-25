# encoding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>

"""
Basic functionality of Omega, such as accessing bodies, materials, interactions.
"""
import unittest
import random
from yade.wrapper import *
from miniWm3Wrap import *
from yade._customConverters import *
from yade import utils
from yade import *

## TODO tests
class TestInteractions(unittest.TestCase): pass
class TestBex(unittest.TestCase): pass
class TestEngines(unittest.TestCase): pass 
class TestIO(unittest.TestCase): pass
class TestTags(unittest.TestCase): pass 

class TestMaterialStateAssociativity(unittest.TestCase):
	def setUp(self): O.reset()
	def testThrowsAtBadCombination(self):
		"throws when body has material and state that don't work together."
		b=Body()
		b.mat=CpmMat()
		b.state=State() #should be CpmState()
		O.bodies.append(b)
		self.assertRaises(RuntimeError,lambda: O.step()) # throws runtime_error
	def testThrowsAtNullState(self):
		"throws when body has material but NULL state."
		b=Body()
		b.mat=Material()
		b.state=None # → shared_ptr<State>() by boost::python
		O.bodies.append(b)
		self.assertRaises(RuntimeError,lambda: O.step())
	def testMaterialReturnsState(self):
		"CpmMat returns CpmState when asked for newAssocState"
		self.assert_(CpmMat().newAssocState().name=='CpmState')

class TestBodies(unittest.TestCase):
	def setUp(self):
		O.reset()
		self.count=100
		O.bodies.append([utils.sphere([random.random(),random.random(),random.random()],random.random()) for i in range(0,self.count)])
		random.seed()
	def testIterate(self):
		"Iteration over O.bodies"
		counted=0
		for b in O.bodies: counted+=1
		self.assert_(counted==self.count)
	def testLen(self):
		"len(O.bodies)"
		self.assert_(len(O.bodies)==self.count)
	def testErase(self):
		"Erased bodies are None in python"
		O.bodies.erase(0)
		self.assert_(O.bodies[0]==None)
	def testNegativeIndex(self):
		"Negative index counts backwards (like python sequences)."
		self.assert_(O.bodies[-1]==O.bodies[self.count-1])
	def testErasedIterate(self):
		"Iterator over O.bodies silently skips erased bodies."
		removed,counted=0,0
		for i in range(0,10):
			id=random.randint(0,self.count-1)
			if O.bodies[id]: O.bodies.erase(id);removed+=1
		for b in O.bodies: counted+=1
		self.assert_(counted==self.count-removed)
		
class TestMaterials(unittest.TestCase):
	def setUp(self):
		# common setup for all tests in this class
		O.reset()
		O.materials.append([
			GranularMat(young=1,label='materialZero'),
			ElasticMat(young=100,label='materialOne')
		])
		O.bodies.append([
			utils.sphere([0,0,0],.5,material=0),
			utils.sphere([1,1,1],.5,material=0),
			utils.sphere([1,1,1],.5,material=1)
		])
	def testShared(self):
		"shared_ptr's makes change in material immediate everywhere"
		O.bodies[0].mat['young']=23423333
		self.assert_(O.bodies[0].mat['young']==O.bodies[1].mat['young'])
	def testSharedAfterReload(self):
		"shared_ptr's are preserved when loading from XML (using a hack in MetaBody::postProcessAttributes)"
		O.saveTmp(); O.loadTmp()
		O.bodies[0].mat['young']=9087438484
		self.assert_(O.bodies[0].mat['young']==O.bodies[1].mat['young'])
	def testLen(self):
		"len(O.materials)"
		self.assert_(len(O.materials)==2)
	def testNegativeIndex(self):
		"Negative index counts backwards."
		self.assert_(O.materials[-1]==O.materials[1])
	def testIterate(self):
		"Iteration over O.materials"
		counted=0
		for m in O.materials: counted+=1
		self.assert_(counted==len(O.materials))
	def testAccess(self):
		"Material found by index or label; KeyError raised for invalid label."
		self.assertRaises(KeyError,lambda: O.materials['nonexistent label'])
		self.assert_(O.materials['materialZero']==O.materials[0])

