#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
##
## TODO: verify that the code for facet & hasShear is physically correct!
##

from math import *
o=Omega()
o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer'),
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingBox2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry',{'hasShear':True,'interactionDetectionFactor':1.4}),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry',{'hasShear':True,'shrinkFactor':1.}),
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	#DeusExMachina('GravityEngine',{'gravity':[0,0,-9.81]}),
	DeusExMachina('RotationEngine',{'subscribedBodies':[1],'rotationAxis':[1,0,0],'angularVelocity':.01}),
	#DeusExMachina('RotationEngine',{'subscribedBodies':[0],'rotationAxis':[1,1,1],'angularVelocity':-.02}),
	DeusExMachina("NewtonsDampedLaw",{'damping':0.2})
]
from yade import utils
scale=.1
o.bodies.append(utils.facet([[scale,0,0],[-scale,-scale,0],[-scale,scale,0]],dynamic=False,color=[1,0,0],young=30e9,poisson=.3))
o.bodies.append(utils.sphere([0,0,.99*scale],1*scale,color=[0,1,0],young=30e9,poisson=.3,density=2400,wire=True,dynamic=False))
o.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

o.dt=.8*utils.PWaveTimeStep()
from yade import qt
renderer=qt.Renderer()
renderer['Interaction_geometry']=True
qt.Controller()
o.step(); o.step(); o.step()
