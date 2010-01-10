#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

O.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	]
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([Ig2_Sphere_Sphere_ScGeom()]),
	InteractionPhysicsDispatcher([Ip2_FrictMat_FrictMat_FrictPhys()]),
	ConstitutiveLawDispatcher([Law2_ScGeom_FrictPhys_Basic()]),
	GravityEngine(gravity=(0,0,-1000)),
	NewtonIntegrator(damping=0.2)
]

for n in range(30):
	O.bodies.append(utils.sphere([0,n,0],.50001,dynamic=(n>0),color=[1-(n/20.),n/20.,0]))
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

