#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

o=Omega()
o.initializers=[
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
	#DeusExMachina('MomentEngine',{'subscribedBodies':[1],'moment':[0,1000,0]}),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-1e2]}),
	DeusExMachina('NewtonsDampedLaw',{'damping':0.2})
]
o.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

from yade import utils
from math import *
for n in range(5):
	o.bodies.append(utils.sphere([0,n,0],.5,dynamic=(n>0),color=[1-(n/20.),n/20.,0],young=30e9,poisson=.3,density=2400))
	# looks for metaengine found in Omega() and uses those
	if n>0: utils.createInteraction(n-1,n)
for i in o.interactions: i.phys['ks']=1e7


o.dt=utils.PWaveTimeStep()
o.saveTmp('init')

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Body_wire']=True
	renderer['Interaction_geometry']=True
except ImportError: pass


#o.save('/tmp/a.xml.bz2')
#o.reload()
#o.run(50000,True)
#print o.iter/o.realtime
