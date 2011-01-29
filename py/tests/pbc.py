# encoding: utf-8
# 2010 © Václav Šmilauer <eudoxos@arcig.cz>

'''
Various computations affected by the periodic boundary conditions.
'''

import unittest
import random,math
from yade.wrapper import *
from miniEigen import *
from yade._customConverters import *
from yade import utils
from yade import *


class TestPBC(unittest.TestCase):
	# prefix test names with PBC: 
	def setUp(self):
		O.reset(); O.periodic=True;
		O.cell.refSize=(2.5,2.5,3)
		self.cellDist=Vector3i(0,0,10) # how many cells away we go
		self.relDist=Vector3(0,.999999999999999999,0) # rel position of the 2nd ball within the cell
		self.initVel=Vector3(0,0,5)
		O.bodies.append(utils.sphere((1,1,1),.5))
		self.initPos=Vector3([O.bodies[0].state.pos[i]+self.relDist[i]+self.cellDist[i]*O.cell.refSize[i] for i in (0,1,2)])
		O.bodies.append(utils.sphere(self.initPos,.5))
		#print O.bodies[1].state.pos
		O.bodies[1].state.vel=self.initVel
		O.engines=[NewtonIntegrator()]
		O.cell.velGrad=Matrix3(0,0,0, 0,0,0, 0,0,-1)
		O.cell.homoDeform=3
		O.dt=0 # do not change positions with dt=0 in NewtonIntegrator, but still update velocities from velGrad
	def testRefSize(self):
		"PBC: hSize reflects changes of refSize"
		O.cell.refSize=(2.55,11,45)
		self.assert_(O.cell.hSize==Matrix3(2.55,0,0, 0,11,0, 0,0,45));
	def testHomotheticResizeVel(self):
		"PBC: homothetic cell deformation adjusts particle velocity (homoDeform==3)"
		O.dt=1e-5
		O.step()
		s1=O.bodies[1].state
		self.assertAlmostEqual(s1.vel[2],self.initVel[2]+self.initPos[2]*O.cell.velGrad[2,2])
	def testHomotheticResizePos(self):
		"PBC: homothetic cell deformation adjusts particle position (homoDeform==1)"
		O.cell.homoDeform=1
		O.step()
		s1=O.bodies[1].state
		self.assertAlmostEqual(s1.vel[2],self.initVel[2])
		self.assertAlmostEqual(s1.pos[2],self.initPos[2]+self.initPos[2]*O.cell.velGrad[2,2]*O.dt)
	def testScGeomIncidentVelocity(self):
		"PBC: ScGeom computes incident velocity correctly (homoDeform==3)"
		O.step()
		O.engines=[InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[])]
		i=utils.createInteraction(0,1)
		self.assertEqual(self.initVel,i.geom.incidentVel(i,avoidGranularRatcheting=True))
		self.assertEqual(self.initVel,i.geom.incidentVel(i,avoidGranularRatcheting=False))
		self.assertAlmostEqual(self.relDist[1],1-i.geom.penetrationDepth)
	def testScGeomIncidentVelocity_homoPos(self):
		"PBC: ScGeom computes incident velocity correctly (homoDeform==1)"
		O.cell.homoDeform=1
		O.step()
		O.engines=[InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[])]
		i=utils.createInteraction(0,1)
		self.assertEqual(self.initVel,i.geom.incidentVel(i,avoidGranularRatcheting=True))
		self.assertEqual(self.initVel,i.geom.incidentVel(i,avoidGranularRatcheting=False))
		self.assertAlmostEqual(self.relDist[1],1-i.geom.penetrationDepth)
	def testL3GeomIncidentVelocity(self):
		"PBC: L3Geom computes incident velocity correctly (homoDeform==3)"
		O.step()
		O.engines=[ForceResetter(),InteractionLoop([Ig2_Sphere_Sphere_L3Geom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl(noBreak=True)]),NewtonIntegrator()]
		i=utils.createInteraction(0,1) 
		O.dt=1e-10; O.step() # tiny timestep, to not move the normal too much
		self.assertAlmostEqual(self.initVel.norm(),(i.geom.u/O.dt).norm())
	def testL3GeomIncidentVelocity_homoPos(self):
		"PBC: L3Geom computes incident velocity correctly (homoDeform==1)"
		O.cell.homoDeform=1; O.step()
		O.engines=[ForceResetter(),InteractionLoop([Ig2_Sphere_Sphere_L3Geom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L3Geom_FrictPhys_ElPerfPl(noBreak=True)]),NewtonIntegrator()]
		i=utils.createInteraction(0,1) 
		O.dt=1e-10; O.step()
		self.assertAlmostEqual(self.initVel.norm(),(i.geom.u/O.dt).norm())
		#self.assertAlmostEqual(self.relDist[1],1-i.geom.penetrationDepth)
	def testKineticEnergy(self):
		"PBC: utils.kineticEnergy considers only fluctuation velocity, not the velocity gradient (homoDeform==3)"
		O.step() # updates velocity with homotheticCellResize
		# ½(mv²+ωIω)
		# #0 is still, no need to add it; #1 has zero angular velocity
		# we must take self.initVel since O.bodies[1].state.vel now contains the homothetic resize which utils.kineticEnergy is supposed to compensate back 
		Ek=.5*O.bodies[1].state.mass*self.initVel.squaredNorm()
		self.assertAlmostEqual(Ek,utils.kineticEnergy())
	def testKineticEnergy_homoPos(self):
		"PBC: utils.kineticEnergy considers only fluctuation velocity, not the velocity gradient (homoDeform==1)"
		O.cell.homoDeform=1; O.step()
		self.assertAlmostEqual(.5*O.bodies[1].state.mass*self.initVel.squaredNorm(),utils.kineticEnergy())


