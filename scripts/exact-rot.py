#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
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
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry',{'exactRot':True}),
		EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-9.81]}),
	DeusExMachina('RotationEngine',{'subscribedBodies':[1],'rotationAxis':[1,0,0],'angularVelocity':.01}),
	DeusExMachina('RotationEngine',{'subscribedBodies':[0],'rotationAxis':[1,1,1],'angularVelocity':-.02}),
	MetaEngine('PhysicalActionDamper',[
		EngineUnit('CundallNonViscousForceDamping',{'damping':0.2}),
		EngineUnit('CundallNonViscousMomentumDamping',{'damping':0.2})
	]),
	MetaEngine('PhysicalActionApplier',[
		EngineUnit('NewtonsForceLaw'),
		EngineUnit('NewtonsMomentumLaw'),
	]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogPositionIntegrator')]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogOrientationIntegrator')]),
]
from yade import utils
o.bodies.append(utils.sphere([0,0,0],1,dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400,wire=True))
o.bodies.append(utils.sphere([0,sqrt(2),sqrt(2)],1,color=[0,1,0],young=30e9,poisson=.3,density=2400,wire=True))
o.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

o.dt=.8*utils.PWaveTimeStep()
#o.save('/tmp/a.xml.bz2');
#o.run(100000); o.wait(); print o.iter/o.realtime,'iterations/sec'
from yade import qt
renderer=qt.Renderer()
renderer['Interaction_geometry']=True
qt.Controller()
o.step(); o.step(); o.step()
