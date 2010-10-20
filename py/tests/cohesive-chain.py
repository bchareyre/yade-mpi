# encoding: utf-8
# 2010 © Bruno Chareyre <bruno.chareyre@grenoble-inp.fr>

'''
Motion of a "sinusoidal" beam made of cylinders
'''

import unittest
import random
from yade.wrapper import *
from miniEigen import *
from yade._customConverters import *
from yade import utils
from yade import *
from math import *

class TestCohesiveChain(unittest.TestCase):
	# prefix test names with PBC: 
	def setUp(self):
		O.reset();
		young=1.0e3
		poisson=5
		density=2.60e3 
		frictionAngle=radians(30)
		O.materials.append(CohFrictMat(young=young,poisson=poisson,density=density,frictionAngle=frictionAngle))
		O.dt=1e-3
		O.engines=[
			ForceResetter(),
			InsertionSortCollider([
			Bo1_ChainedCylinder_Aabb(),
			Bo1_Sphere_Aabb()]),
		InteractionLoop(
			[Ig2_ChainedCylinder_ChainedCylinder_ScGeom(),Ig2_Sphere_ChainedCylinder_CylScGeom()],
			[Ip2_2xCohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True,normalCohesion=1e13,shearCohesion=1e13)],
			[Law2_ScGeom_CohFrictPhys_CohesionMoment(momentRotationLaw=True)]),
		## Apply gravity
		GravityEngine(gravity=[0,-9.81,0]),
		## Motion equation
		NewtonIntegrator(damping=0.15)
		]
		#Generate a spiral
		Ne=10
		for i in range(0, Ne):
			omeg=95.0/float(Ne); hy=0.05; hz=0.07;
			px=float(i)*(omeg/60.0); py=sin(float(i)*omeg)*hy; pz=cos(float(i)*omeg)*hz;
			px2=float(i+1.)*(omeg/60.0); py2=sin(float(i+1.)*omeg)*hy; pz2=cos(float(i+1.)*omeg)*hz;
			O.bodies.append(utils.chainedCylinder(begin=Vector3(pz,py,px), radius=0.005,end=Vector3(pz2,py2,px2),color=Vector3(0.6,0.5,0.5)))		
		O.bodies[Ne-1].state.blockedDOFs=['x','y','z','rx','ry','rz']
	def testMotion(self):
		"CohesiveChain: velocity/positions tested in transient dynamics and equilibrium state"
		#target values
		tv1=-0.881733955694;tp1=-0.0504220815057;tv2=-0.0007511382705;tp2=-0.07474048423;
		tolerance = 10e-3
		O.run(100,True)
		v1=O.bodies[0].state.vel[1];p1=O.bodies[0].state.pos[1]
		#print v1,p1
		O.run(10000,True)
		v2=O.bodies[0].state.vel[1];p2=O.bodies[0].state.pos[1]
		#print v2,p2
		self.assertTrue(abs(tv1-v1)<abs(tolerance*tv1) and abs(tp1-p1)<abs(tolerance*tp1))
		self.assertTrue(abs(tv2-v2)<abs(tolerance*tv2) and abs(tp2-p2)<abs(tolerance*tp2))
