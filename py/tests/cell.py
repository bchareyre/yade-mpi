# encoding: utf-8
# 2010 © Václav Šmilauer <eudoxos@arcig.cz>

'''
Various computations affected by the periodic boundary conditions.
'''

import unittest
import random
from yade.wrapper import *
from miniEigen import *
from yade._customConverters import *
from yade import utils
from yade import *


class TestCell(unittest.TestCase):
	def setUp(self):
		O.reset(); O.periodic=True;
		O.cell.refSize=Vector3(2.5,2.5,3)
		self.cellDist=Vector3i(0,0,10) # how many cells away we go
		self.relDist=Vector3(0,.999,0) # rel position of the 2nd ball within the cell
		self.initVel=Vector3(0,0,5)
		O.bodies.append(utils.sphere((1,1,1),.5))
		self.initPos=Vector3([O.bodies[0].state.pos[i]+self.relDist[i]+self.cellDist[i]*O.cell.refSize[i] for i in (0,1,2)])
		O.bodies.append(utils.sphere(self.initPos,.5))
		print O.bodies[1].state.pos
		O.bodies[1].state.vel=self.initVel
		O.dt=1e-5
		O.engines=[NewtonIntegrator(homotheticCellResize=2)]
		O.cell.velGrad=Matrix3(0,0,0, 0,0,0, 0,0,-1)
	def testHomotheticResize(self):
		"Homothetic cell resize adjusts particle velocity"
		O.step()
		s1=O.bodies[1].state
		self.assertAlmostEqual(s1.vel[2],self.initVel[2]+self.initPos[2]*O.cell.velGrad[2,2])
	def testScGeomIncidentVelocity(self):
		O.step()
		O.engines=[InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[])]
		i=utils.createInteraction(0,1)
		if 0:
			print i.geom.incidentVel(i,avoidGranularRatcheting=True)
			print i.geom.incidentVel(i,avoidGranularRatcheting=False)
			print i.geom.penetrationDepth
			print i.geom.dict()
			print '=========='
			print 'v1',O.bodies[1].state.vel
			print 'shiftVel',O.cell.velGrad*O.cell.Hsize*(0,0,-10)

	def testKineticEnergy(self):
		O.step() # updates velocity with homotheticCellResize
		# ½(mv²+ωIω)
		# #0 is still, no need to add it; #1 has zero angular velocity
		# we must take self.initVel since O.bodies[1].state.vel now contains the homothetic resize which utils.kineticEnergy is supposed to compensate back 
		Ek=.5*O.bodies[1].state.mass*self.initVel.squaredNorm()


