# encoding: utf-8
# 2009 © Václav Šmilauer <eudoxos@arcig.cz>

"""
Basic functionality of Omega, such as accessing bodies, materials, interactions.
"""
import unittest
import random
from yade.wrapper import *
from miniEigen import *
from yade._customConverters import *
from yade import utils
from yade import *

## TODO tests
class TestInteractions(unittest.TestCase): pass
class TestForce(unittest.TestCase): pass
class TestEngines(unittest.TestCase): pass 
class TestTags(unittest.TestCase): pass 

class TestIO(unittest.TestCase):
	def testSaveAllClasses(self):
		'I/O: All classes can be saved and loaded with boost::serialization'
		import yade.system
		failed=set()
		for c in yade.system.childClasses('Serializable'):
			O.reset()
			try:
				O.miscParams=[eval(c)()]
				O.saveTmp()
				O.loadTmp()
			except (RuntimeError,ValueError):
				failed.add(c)
		failed=list(failed); failed.sort()
		self.assert_(len(failed)==0,'Failed classes were: '+' '.join(failed))



class TestCell(unittest.TestCase):
	def setUp(self):
		O.reset(); O.periodic=True
	def testAttributesAreCrossUpdated(self):
		"Cell: updates Hsize automatically when refSize is updated"
		O.cell.refSize=(2.55,11,45)
		self.assert_(O.cell.Hsize==Matrix3(2.55,0,0, 0,11,0, 0,0,45));

class TestMaterialStateAssociativity(unittest.TestCase):
	def setUp(self): O.reset()
	def testThrowsAtBadCombination(self):
		"Material+State: throws when body has material and state that don't work together."
		b=Body()
		b.mat=CpmMat()
		b.state=State() #should be CpmState()
		O.bodies.append(b)
		self.assertRaises(RuntimeError,lambda: O.step()) # throws runtime_error
	def testThrowsAtNullState(self):
		"Material+State: throws when body has material but NULL state."
		b=Body()
		b.mat=Material()
		b.state=None # → shared_ptr<State>() by boost::python
		O.bodies.append(b)
		self.assertRaises(RuntimeError,lambda: O.step())
	def testMaterialReturnsState(self):
		"Material+State: CpmMat returns CpmState when asked for newAssocState"
		self.assert_(CpmMat().newAssocState().__class__==CpmState)

class TestBodies(unittest.TestCase):
	def setUp(self):
		O.reset()
		self.count=100
		O.bodies.append([utils.sphere([random.random(),random.random(),random.random()],random.random()) for i in range(0,self.count)])
		random.seed()
	def testIterate(self):
		"Bodies: Iteration"
		counted=0
		for b in O.bodies: counted+=1
		self.assert_(counted==self.count)
	def testLen(self):
		"Bodies: len(O.bodies)"
		self.assert_(len(O.bodies)==self.count)
	def testErase(self):
		"Bodies: erased bodies are None in python"
		O.bodies.erase(0)
		self.assert_(O.bodies[0]==None)
	def testNegativeIndex(self):
		"Bodies: Negative index counts backwards (like python sequences)."
		self.assert_(O.bodies[-1]==O.bodies[self.count-1])
	def testErasedIterate(self):
		"Bodies: Iterator silently skips erased ones"
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
			FrictMat(young=1,label='materialZero'),
			ElastMat(young=100,label='materialOne')
		])
		O.bodies.append([
			utils.sphere([0,0,0],.5,material=0),
			utils.sphere([1,1,1],.5,material=0),
			utils.sphere([1,1,1],.5,material=1)
		])
	def testShared(self):
		"Material: shared_ptr's makes change in material immediate everywhere"
		O.bodies[0].mat.young=23423333
		self.assert_(O.bodies[0].mat.young==O.bodies[1].mat.young)
	def testSharedAfterReload(self):
		"Material: shared_ptr's are preserved when saving/loading"
		O.saveTmp(); O.loadTmp()
		O.bodies[0].mat.young=9087438484
		self.assert_(O.bodies[0].mat.young==O.bodies[1].mat.young)
	def testLen(self):
		"Material: len(O.materials)"
		self.assert_(len(O.materials)==2)
	def testNegativeIndex(self):
		"Material: negative index counts backwards."
		self.assert_(O.materials[-1]==O.materials[1])
	def testIterate(self):
		"Material: iteration over O.materials"
		counted=0
		for m in O.materials: counted+=1
		self.assert_(counted==len(O.materials))
	def testAccess(self):
		"Material: find by index or label; KeyError raised for invalid label."
		self.assertRaises(KeyError,lambda: O.materials['nonexistent label'])
		self.assert_(O.materials['materialZero']==O.materials[0])

