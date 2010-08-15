#!/usr/bin/python
# -*- coding: utf-8 -*-
"""This example demonstrates GTS (http://gts.sourceforge.net/) opportunities for creating surfaces
VTU-files are created in /tmp directory after simulation. If you open those with paraview
(or other VTK-based) program, you can create video, make screenshots etc."""

from numpy import linspace
from yade import pack
thetas=linspace(0,2*pi,num=16,endpoint=True)
meridians=pack.revolutionSurfaceMeridians([[(3+rad*sin(th),10*rad+rad*cos(th)) for th in thetas] for rad in linspace(1,2,num=10)],linspace(0,pi,num=10))
surf=pack.sweptPolylines2gtsSurface(meridians+[[Vector3(5*sin(-th),-10+5*cos(-th),30) for th in thetas]])
O.bodies.append(pack.gtsSurface2Facets(surf))

sp=pack.SpherePack()
sp.makeCloud(Vector3(-1,-9,30),Vector3(1,-13,32),.2,rRelFuzz=.3)
O.bodies.append([utils.sphere(c,r) for c,r in sp])

O.engines=[
	ForceResetter(), 
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionDispatchers(
	[Ig2_Sphere_Sphere_Dem3DofGeom(),
		Ig2_Facet_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(),
	VTKRecorder(iterPeriod=100,recorders=['spheres','facets','colors'],fileName='/tmp/p1-')
]
O.dt=utils.PWaveTimeStep()


from yade import qt
qt.Controller()
qt.View()

O.run(8500)
