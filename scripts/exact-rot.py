#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
from math import *
O.initializers=[
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()]),
	]
O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([
		InteractingSphere2InteractingSphere4SpheresContactGeometry(),
		InteractingFacet2InteractingSphere4SpheresContactGeometry(),
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
