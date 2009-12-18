#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
from math import *
O.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()])
]
O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([
		Ig2_Sphere_Sphere_ScGeom(),
		Ig2_Facet_Sphere_ScGeom(),
	]),
	InteractionPhysicsDispatcher([SimpleElasticRelationships()]),
	ElasticContactLaw(),
	GravityEngine(gravity=(0,0,-9.81)),
	RotationEngine(subscribedBodies=[1],rotationAxis=(1,0,0),angularVelocity=.01),
	RotationEngine(subscribedBodies=[0],rotationAxis=(1,1,1),angularVelocity=-.02),
	NewtonIntegrator(damping=.2)
]
from yade import utils
O.bodies.append(utils.sphere([0,0,0],1,dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400,wire=True))
O.bodies.append(utils.sphere([0,sqrt(2),sqrt(2)],1,color=[0,1,0],young=30e9,poisson=.3,density=2400,wire=True))

O.dt=.8*utils.PWaveTimeStep()
O.saveTmp()
#o.run(100000); o.wait(); print o.iter/o.realtime,'iterations/sec'
from yade import qt
renderer=qt.Renderer()
renderer['Interaction_geometry']=True
qt.Controller()
O.step(); O.step(); O.step()
