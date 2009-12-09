#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
##
## TODO: verify that the code for facet & hasShear is physically correct!
##

from math import *
O.initializers=[
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB(),InteractingFacet2AABB()])
	]
O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB(),InteractingFacet2AABB()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([
		InteractingSphere2InteractingSphere4SpheresContactGeometry(),
		InteractingFacet2InteractingSphere4SpheresContactGeometry(),
	]),
	InteractionPhysicsDispatcher([SimpleElasticRelationships()]),
	ElasticContactLaw(),
	RotationEngine(subscribedBodies=[1],rotationAxis=[1,0,0],angularVelocity=.01),
	NewtonIntegrator(damping=0.2)
]
from yade import utils
scale=.1
O.bodies.append(utils.facet([[scale,0,0],[-scale,-scale,0],[-scale,scale,0]],dynamic=False,color=[1,0,0],young=30e9,poisson=.3))
O.bodies.append(utils.sphere([0,0,.99*scale],1*scale,color=[0,1,0],young=30e9,poisson=.3,density=2400,wire=True,dynamic=False))

O.dt=.8*utils.PWaveTimeStep()
from yade import qt
renderer=qt.Renderer()
renderer['Interaction_geometry']=True
qt.Controller()
O.step(); O.step(); O.step()
