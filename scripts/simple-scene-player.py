#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
o=Omega() 
o.initializers=[
		BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()])
]
o.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[SimpleElasticRelationships()],
		[ef2_Spheres_Elastic_ElasticLaw()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=.2),
	###
	### NOTE this extra engine.
	###
	SQLiteRecorder(recorders=['se3','rgb'],dbFile='/tmp/player.sqlite',iterPeriod=50)
]
from yade import utils
o.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
o.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
o.dt=.4*utils.PWaveTimeStep()

o.run(15000)
o.wait()

def setWire():
	o=Omega()
	for b in o.bodies: b.shape['wire']=False
# you could have saved the viewer state by using Alt-S in the view...
from yade import qt
qt.makePlayerVideo('/tmp/player.sqlite','/tmp/player.ogg','/tmp/qglviewerState.xml',stride=10,fps=12,postLoadHook='setWire()')

quit()
