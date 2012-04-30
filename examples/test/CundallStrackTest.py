# -*- coding: utf-8 -*-


O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),]),
	IGeomDispatcher([Ig2_Sphere_Sphere_Dem3DofGeom()]),
	IPhysDispatcher([Ip2_2xFrictMat_CSPhys()]),
	LawDispatcher([Law2_Dem3Dof_CSPhys_CundallStrack()]),
	NewtonIntegrator(damping = 0.01,gravity=[0,0,-9.81])
]

from yade import utils

O.bodies.append(utils.sphere([0,0,6],1,fixed=False, color=[0,1,0]))
O.bodies.append(utils.sphere([0,0,0],1,fixed=True, color=[0,0,1]))
O.dt=.2*utils.PWaveTimeStep()

from yade import qt
qt.Controller()
qt.View()
