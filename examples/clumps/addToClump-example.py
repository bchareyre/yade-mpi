#!/usr/bin/python
# -*- coding: utf-8 -*-

'''This example shows usage of addToClump() and appendClumped().'''

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
id_box = O.bodies.append(box((0,0,0),(2,2,.1),fixed=True,material=Mat))


#### show how to use appendClumped():


#create 2 clumps:
clump1=O.bodies.appendClumped([\
sphere([0,0,1], material=Mat, radius=0.5),\
sphere([0.2,0,1], material=Mat, radius=0.5)\
])
clump2=O.bodies.appendClumped([\
sphere([3,1,2], material=Mat, radius=0.5),\
sphere([3.2,1,2], material=Mat, radius=0.5)\
])

#get clump ids:
id_clump1 = clump1[0]
id_clump2 = clump2[0]

#definition for getting informations from all clumps:
def getClumpInfo():
	for b in O.bodies:
		if b.isClump:
			print 'Clump ',b.id,' has following members:'
			keys = b.shape.members.keys()
			for ii in range(0,len(keys)):
				print '- Body ',keys[ii]
			print 'inertia:',b.state.inertia
			print 'mass:',b.state.mass,'\n'


#### show how to use addToClump():


#create a new sphere:
id_new=O.bodies.append(sphere([0,0.2,1], material=Mat, radius=0.5))

print '\nSTATE before adding sphere to clump ------------'
getClumpInfo()

#add a sphere to the clump:
O.bodies.addToClump([id_new],id_clump1)

print '\nSTATE after adding sphere to clump ------------'
getClumpInfo()

#add a clump to a clump:
O.bodies.addToClump([id_clump2],id_clump1)

print '\nSTATE after adding the second clump to clump ------------'
getClumpInfo()

#try to add clump member to a clump (should give error message):
#O.bodies.addToClump(1,id_clump1)
#try to add clump to the same clump (should give error message):
#O.bodies.addToClump(id_clump1,id_clump1)

O.dt=1e-6

print '\nPress Play button ... '
