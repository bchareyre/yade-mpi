#!/usr/bin/python
# -*- coding: utf-8 -*-

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GravityEngine(gravity=(0,0,-1000)),
	NewtonIntegrator(damping=0.2)
]

for n in range(30):
	O.bodies.append(utils.sphere([0,n,0],.50001,dynamic=(n>0),color=[1-(n/20.),n/20.,0]))
O.bodies[len(O.bodies)-1].dynamic=False
#	# looks for metaengine found in Omega() and uses those
#	if n>0: utils.createInteraction(n-1,n)
#for i in O.interactions: i.phys['ks']=1e9

O.dt=utils.PWaveTimeStep()*0.001
O.saveTmp('init')

try:
	from yade import qt
	qt.View()
	renderer=qt.Renderer()
	renderer.wire=True
	renderer.intrGeom=True
except ImportError: pass

O.run(100000,True)

