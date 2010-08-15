#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),]),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_Basic()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=.2)
]

O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
O.dt=.4*utils.PWaveTimeStep()

from yade import qt
O.stopAtIter=15000
qt.makeSimulationVideo('/tmp/aa.ogg',iterPeriod=100,fps=12)





