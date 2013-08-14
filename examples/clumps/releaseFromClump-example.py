#!/usr/bin/python
# -*- coding: utf-8 -*-

'''This example shows usage of releaseFromClump() and clump().'''

#define material for all bodies:
id_Mat=O.materials.append(FrictMat(young=1e6,poisson=0.3,density=1000,frictionAngle=1))
Mat=O.materials[id_Mat]

#define engines:
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(damping=0.7,gravity=[0,0,-10])
]

#create a box:
id_box = O.bodies.append(box((0,0,0),(5,5,.1),fixed=True,material=Mat))


#### show how to use clump():


#create spheres parallel to x-axis:
bodyList1 = []
for ii in range(-2,2):
	bodyList1.append(O.bodies.append(sphere((ii,0,1),radius=.5,material=Mat)))

#create spheres parallel to y-axis:
bodyList2 = []
for ii in range(-3,2):
	bodyList2.append(O.bodies.append(sphere((-1,ii,1.5),radius=.5,material=Mat)))

#create 2 clumps, and give each clump a different color
idClump1=O.bodies.clump(bodyList1)
idClump2=O.bodies.clump(bodyList2)
for ii in bodyList1:
	O.bodies[ii].shape.color=(.1,.5,.1)
for ii in bodyList2:
	O.bodies[ii].shape.color=(.2,.2,.7)

#definition for getting informations from all clumps:
def getClumpInfo():
	for b in O.bodies:
		if b.isClump:
			print 'Clump ',b.id,' has following members:'
			keys = b.shape.members.keys()
			for ii in range(0,len(keys)):
				print '- Body ',keys[ii]

O.dt=1e-6


#### show how to use releaseFromClump():


print '\nSTATE before releasing spheres from clump ------------'
getClumpInfo()

raw_input('\nPress Play button ... and look what happens.\n Then press Pause button and press ENTER on console!')

for ii in bodyList2:
	if ii > max(bodyList2)-2:
		clId = O.bodies[ii].clumpId
		O.bodies.releaseFromClump(ii,clId)
		print 'Sphere ',ii,' released from clump ',clId

print '\nSTATE after releasing spheres from clump ------------'
getClumpInfo()

print '\nPress Play button again ... '
