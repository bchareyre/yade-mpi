#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
##
## TODO: verify that the code for facet & hasShear is physically correct!
##

from math import *
O.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),Bo1_Facet_Aabb()])
	]
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([
		Ig2_Sphere_Sphere_ScGeom(),
		Ig2_Facet_Sphere_ScGeom(),
	]),
	InteractionPhysicsDispatcher([Ip2_FrictMat_FrictMat_FrictPhys()]),
	ElasticContactLaw(),
	RotationEngine(subscribedBodies=[1],rotationAxis=[1,0,0],angularVelocity=.01),
	NewtonIntegrator(damping=0.2)
]
from yade import utils
scale=.1
O.bodies.append(utils.facet([[scale,0,0],[-scale,-scale,0],[-scale,scale,0]],dynamic=False,color=[1,0,0]))
O.bodies.append(utils.sphere([0,0,.99*scale],1*scale,color=[0,1,0],wire=True,dynamic=False))

O.dt=.4*utils.PWaveTimeStep()
from yade import qt
qt.View()
renderer=qt.Renderer()
renderer.intrGeom=True
qt.Controller()
O.step(); O.step(); O.step()
O.run(20000)
