#!/usr/bin/python
# -*- coding: utf-8 -*-
# © 2012 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
"""Script showing how large bodies can be combined with periodic boundary conditions using InsertioSortCollider::allowBiggerThanPeriod=True (1)."""

from yade import pack
from pylab import rand
from yade import qt

O.periodic=True
length=0.4
height=0.6
width=0.2
thickness=0.01

O.cell.hSize=Matrix3(length, 0, 0,
		     0 ,3.*height, 0,
		    0, 0, width)

O.materials.append(FrictMat(density=1,young=1e5,poisson=0.3,frictionAngle=radians(30),label='boxMat'))
lowBox = box( center=(length/2.0,height-thickness/2.0,width/2.0), extents=(length*1000.0,thickness/2.0,width*1000.0) ,fixed=True,wire=False)
O.bodies.append(lowBox)

radius=0.01
O.materials.append(FrictMat(density=1000,young=1e4,poisson=0.3,frictionAngle=radians(30),label='sphereMat'))
sp=pack.SpherePack()
sp.makeCloud((0.*length,height+1.2*radius,0.25*width),(0.5*length,2*height-1.2*radius,0.75*width),-1,.2,2000,periodic=True)
O.bodies.append([sphere(s[0],s[1],color=(0.6+0.15*rand(),0.5+0.15*rand(),0.15+0.15*rand())) for s in sp])

O.dt=0.2*PWaveTimeStep()
O.usesTimeStepper=True
newton=NewtonIntegrator(damping=0.6,gravity=(0,-10,0))

O.engines=[
	ForceResetter(),
	#(1) This is where we all big bodies, else it would crash due to the very large bottom box:
	InsertionSortCollider([Bo1_Box_Aabb(),Bo1_Sphere_Aabb()],allowBiggerThanPeriod=True),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8,defaultDt=O.dt),
	newton
]

Gl1_Sphere.stripes=1

from yade import qt
qt.View()
print('Press PLAY button')



