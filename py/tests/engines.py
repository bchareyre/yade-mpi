'test functionality of individual engines'

import unittest
import random
from yade.wrapper import *
from yade._customConverters import *
from yade import utils
from yade import *
from math import *
from minieigen import *

class TestKinematicEngines(unittest.TestCase):
	def testKinematicEngines(self):
		'Engines: kinematic engines'
		tolerance = 1e-5
		rotIndex=1.0
		angVelTemp = pi/rotIndex
		O.reset()
		id_fixed_transl = O.bodies.append(utils.sphere((0.0,0.0,0.0),1.0,fixed=True))
		id_nonfixed_transl = O.bodies.append(utils.sphere((0.0,5.0,0.0),1.0,fixed=False))
		id_fixed_rot = O.bodies.append(utils.sphere((0.0,10.0,10.0),1.0,fixed=True))
		id_nonfixed_rot = O.bodies.append(utils.sphere((0.0,15.0,10.0),1.0,fixed=False))
		id_fixed_helix = O.bodies.append(utils.sphere((0.0,20.0,10.0),1.0,fixed=True))
		id_nonfixed_helix = O.bodies.append(utils.sphere((0.0,25.0,10.0),1.0,fixed=False))
		O.engines=[
			TranslationEngine(velocity = 1.0, translationAxis = [1.0,0,0], ids = [id_fixed_transl]),
			TranslationEngine(velocity = 1.0, translationAxis = [1.0,0,0], ids = [id_nonfixed_transl]),
			RotationEngine(angularVelocity = pi/angVelTemp, rotationAxis = [0.0,1.0,0.0], rotateAroundZero = True, zeroPoint = [0.0,0.0,0.0], ids = [id_fixed_rot]),
			RotationEngine(angularVelocity = pi/angVelTemp, rotationAxis = [0.0,1.0,0.0], rotateAroundZero = True, zeroPoint = [0.0,5.0,0.0], ids = [id_nonfixed_rot]),
			HelixEngine(angularVelocity = pi/angVelTemp, rotationAxis = [0.0,1.0,0.0], linearVelocity = 1.0, zeroPoint = [0.0,0.0,0.0], ids = [id_fixed_helix]),
			HelixEngine(angularVelocity = pi/angVelTemp, rotationAxis = [0.0,1.0,0.0], linearVelocity = 1.0, zeroPoint = [0.0,5.0,0.0], ids = [id_nonfixed_helix]),
			ForceResetter(),
			NewtonIntegrator()
		]
		O.dt = 1.0
		for i in range(0,2):
			O.step()
			self.assertTrue(abs(O.bodies[id_fixed_transl].state.pos[0] - O.iter) < tolerance)												#Check translation of fixed bodies
			self.assertTrue(abs(O.bodies[id_nonfixed_transl].state.pos[0] - O.iter) < tolerance)										#Check translation of nonfixed bodies
			
			self.assertTrue(abs(O.bodies[id_fixed_rot].state.pos[0]-10.0*sin(pi/angVelTemp*O.iter))<tolerance)			#Check rotation of fixed bodies X
			self.assertTrue(abs(O.bodies[id_fixed_rot].state.pos[2]-10.0*cos(pi/angVelTemp*O.iter))<tolerance)			#Check rotation of fixed bodies Y
			self.assertTrue(abs(O.bodies[id_fixed_rot].state.ori.toAxisAngle()[1]-Quaternion(Vector3(0.0,1.0,0.0),pi/angVelTemp*O.iter).toAxisAngle()[1])<tolerance)		#Check rotation of fixed bodies, angle
			
			self.assertTrue(abs(O.bodies[id_nonfixed_rot].state.pos[0] - 10*sin(pi/angVelTemp*O.iter))<tolerance)		#Check rotation of nonfixed bodies X
			self.assertTrue(abs(O.bodies[id_nonfixed_rot].state.pos[2] - 10*cos(pi/angVelTemp*O.iter))<tolerance)		#Check rotation of nonfixed bodies Y
			self.assertTrue(abs(O.bodies[id_nonfixed_rot].state.ori.toAxisAngle()[1]-Quaternion(Vector3(0.0,1.0,0.0),pi/angVelTemp*O.iter).toAxisAngle()[1])<tolerance)		#Check rotation of nonfixed bodies, angle
						
			self.assertTrue(abs(O.bodies[id_fixed_helix].state.pos[0] - 10*sin(pi/angVelTemp*O.iter))<tolerance)		#Check helixEngine of fixed bodies X
			self.assertTrue(abs(O.bodies[id_fixed_helix].state.pos[2] - 10*cos(pi/angVelTemp*O.iter))<tolerance)		#Check helixEngine of fixed bodies Y
			self.assertTrue(abs(O.bodies[id_fixed_helix].state.pos[1]-20.0 - O.iter)<tolerance)		#Check helixEngine of fixed bodies Z
			
			self.assertTrue(abs(O.bodies[id_nonfixed_helix].state.pos[0] - 10*sin(pi/angVelTemp*O.iter))<tolerance)		#Check helixEngine of nonfixed bodies X
			self.assertTrue(abs(O.bodies[id_nonfixed_helix].state.pos[2] - 10*cos(pi/angVelTemp*O.iter))<tolerance)		#Check helixEngine of nonfixed bodies Y
			self.assertTrue(abs(O.bodies[id_nonfixed_helix].state.pos[1]-25.0 - O.iter)<tolerance)		#Check helixEngine of nonfixed bodies Z

class TestLabelsOfEngines(unittest.TestCase):
	def testLabels(self):
		O.reset()
		resetter = ForceResetter(label='lresetter')
		bo1 = Bo1_Sphere_Aabb(label='lbo1')
		collider = InsertionSortCollider([bo1],label='lcollider')
		ig2 = Ig2_Sphere_Sphere_ScGeom(label='lig2')
		ip2 = Ip2_FrictMat_FrictMat_FrictPhys(label='lip2')
		law2 = Law2_ScGeom_FrictPhys_CundallStrack(label='llaw2')
		iLoop = InteractionLoop([ig2],[ip2],[law2],label='liLoop')
		trans1 = TranslationEngine(label='ltrans1')
		rot1 = RotationEngine(label='lrot1')
		trans2 = TranslationEngine(label='ltrans2')
		rot2 = RotationEngine(label='lrot2')
		comb = trans2 + rot2
		comb.label = 'lcomb'
		newton = NewtonIntegrator(label='lnewton')
		pyRunner = PyRunner(label='lpyRunner')
		O.engines=[
			resetter,
			collider,
			iLoop,
			trans1,
			rot1,
			comb,
			newton,
			pyRunner,
		]
		self.assertEqual(resetter,lresetter)
		self.assertEqual(bo1,lbo1)
		self.assertEqual(collider,lcollider)
		self.assertEqual(ig2,lig2)
		self.assertEqual(ip2,lip2)
		self.assertEqual(law2,llaw2)
		self.assertEqual(iLoop,liLoop)
		self.assertEqual(trans1,ltrans1)
		self.assertEqual(rot1,lrot1)
		self.assertEqual(comb,lcomb)
		self.assertEqual(trans2,ltrans2)
		self.assertEqual(rot2,lrot2)
		self.assertEqual(newton,lnewton)
		self.assertEqual(pyRunner,lpyRunner)
