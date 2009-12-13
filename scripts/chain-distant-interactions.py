#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

o=Omega()
o.initializers=[
	BoundDispatcher([
		InteractingSphere2AABB(),InteractingBox2AABB()
	]
o.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingBox2AABB()])
	InsertionSortCollider(),
	InteractionGeometryDispatcher([Ig2_Sphere_Sphere_ScGeom(hasShear=True)]),
	InteractionPhysicsDispatcher([SimpleElasticRelationships()]),
	ElasticContactLaw(isCohesive=True),
	MomentEngine(subscribedBodies=[1],moment=(0,1000,0)),
	GravityEngine(gravity=(0,0,1e-2)),
	NewtonIntegrator(damping=0.2)
]

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
