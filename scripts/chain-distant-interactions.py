#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

o=Omega()
o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer'),
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider',{'haveDistantTransient':True}),
	MetaEngine('InteractionGeometryMetaEngine',[EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry',{'hasShear':True}),]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw2',{'isCohesive':True}),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-1e5]}),
	DeusExMachina('NewtonsDampedLaw',{'damping':0.1})
]
from yade import utils
for n in range(20):
	o.bodies.append(utils.sphere([0,n,0],.5,dynamic=(n>0),color=[1-(n/20.),n/20.,0],young=30e9,poisson=.3,density=2400))
	# looks for metaengine found in Omega() and uses those
	if n>0: utils.createInteraction(n-1,n)


o.dt=.04*utils.PWaveTimeStep()
#o.save('/tmp/a.xml.bz2')
#o.reload()
#o.run(50000,True)
#print o.iter/o.realtime
