#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

O.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
O.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider',{'haveDistantTransient':True}),
	MetaEngine('InteractionGeometryMetaEngine',[EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry',{'hasShear':True}),]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	MetaEngine('ConstitutiveLawDispatcher',[EngineUnit('ef2_Spheres_NormalShear_ElasticFrictionalLaw')]),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-1000]}),
	DeusExMachina('NewtonsDampedLaw',{'damping':0.2})
]
O.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

for n in range(30):
	O.bodies.append(utils.sphere([0,n,0],.50001,dynamic=(n>0),color=[1-(n/20.),n/20.,0],young=30e9,poisson=.3,density=2400))
O.bodies[len(O.bodies)-1]['isDynamic']=False
#	# looks for metaengine found in Omega() and uses those
#	if n>0: utils.createInteraction(n-1,n)
#for i in O.interactions: i.phys['ks']=1e9

O.dt=utils.PWaveTimeStep()
O.saveTmp('init')

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Body_wire']=True
	renderer['Interaction_geometry']=True
except ImportError: pass
O.run(100,True)

#o.save('/tmp/a.xml.bz2')
#o.reload()
#o.run(50000,True)
#print o.iter/o.realtime
