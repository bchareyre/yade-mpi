#!/usr/local/bin/yade-trunk -x
# -*- coding: utf-8 -*-
#Author : Kneib François, francois.kneib@gmail.com
#This script is designed to show the "circleView" features, which allow to display circles instead of spheres for 2D simulations.
#/!\ this is just a DISPLAY FEATURE, computed particles still are SPHERICAL.

from yade import qt
X=1
Y=1
Z=1
O.periodic=True
O.cell.hSize=Matrix3(	X,	0,	0,
						0,	Y,	0,
						0,	0,	Z)

O.materials.append(FrictMat(density=1000,young=1e5,poisson=0.5,frictionAngle=radians(20),label='sphereMat'))

#Generate the 2D packing. Y is the "flat" axis.
sp=pack.SpherePack()
sp.makeCloud((0.,Y/2.,0.),(X,Y/2.,Z),rMean=X/50.,rRelFuzz=0.1)
sp.toSimulation(material='sphereMat')

O.engines=[
    ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
    InteractionLoop(
		    [Ig2_Sphere_Sphere_ScGeom()],
		    [Ip2_FrictMat_FrictMat_FrictPhys()],
		    [Law2_ScGeom_FrictPhys_CundallStrack()]
    ),
	NewtonIntegrator(damping=0.1, gravity=(0.,0.,-10)),
]
O.dt=2e-06

#Block the "flat" axis (Y), and only allow rotations around it.
for i in O.bodies:
	i.state.blockedDOFs="yXZ"
	i.shape.color=(0.,0.,1.)

#Fix some spheres to make the simulation "interesting".
for i in range(0,int(len(O.bodies)/10)):
	O.bodies[i].state.blockedDOFs="xyzXYZ"
	O.bodies[i].shape.color=(1.,0.,0.)

#Enable 3D view.
qt.View()
#Activate the circle (torus) view
Gl1_Sphere.circleView=1
#Thickness of the circles :
Gl1_Sphere.circleRelThickness=0.3
#Tell the viewer which axis is "flat", so that it can rotate the tori around the two others to make them face to the "flat" axis.
Gl1_Sphere.circleAllowedRotationAxis='y'
#Make the background white
R=yade.qt.Renderer()
R.bgColor=(1.,1.,1.)
R.intrPhys=True







