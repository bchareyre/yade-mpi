#!/usr/bin/python
# -*- coding: utf-8 -*-

'''This example shows usage of adaptClumpMasses().'''

#define material for all bodies:
id_Mat=O.materials.append(FrictMat(young=1e7,poisson=0.3,density=1000,frictionAngle=1))
Mat=O.materials[id_Mat]

#define engines:
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(damping=0.7,gravity=[0,0,-10])
]

#append spheres:
sphereList = []
sphereList.append(O.bodies.append(sphere((0,0,0),radius=.5,material=Mat)))
sphereList.append(O.bodies.append(sphere((.5,0,0),radius=.25,material=Mat)))
sphereList.append(O.bodies.append(sphere((0,.3,0),radius=.5,material=Mat)))
sphereList.append(O.bodies.append(sphere((0,0,.3),radius=.5,material=Mat)))

#clump spheres:
idClump=O.bodies.clump(sphereList)

from yade import qt
qt.Controller()
qt.View()

def printClumpMasses():
	for b in O.bodies:
		if b.isClump:
			print 'Clump ',b.id,' with mass ',b.state.mass,' has following members:'
			keys = b.shape.members.keys()
			for ii in range(0,len(keys)):
				print '- Body ',keys[ii],' with mass ',O.bodies[keys[ii]].state.mass

print 'Mass info BEFORE adaptClumpMasses() is called: ----------------'
printClumpMasses()

massInfo = O.bodies.adaptClumpMasses([],10000000)		#give an empty list [] if no body should be excluded, number of grid points is set to 10000000

#NOTE: the higher the number of grid points, the more precisely is the result, 
#      but high numbers of clumps + high number of grid points = a looooong coffee break ...

print 'Mass info AFTER adaptClumpMasses() is called: -----------------'
printClumpMasses()
print 'output of adaptClumpMasses(): ',massInfo
