#!/usr/bin/python
# -*- coding: utf-8 -*-

from yade import utils
import random
from yade import ymport

## PhysicalParameters 

## Variant of mesh
mesh = 'coarse'
#mesh = 'fine'
#mesh = 'tiny'

## Import geometry 
rod = O.bodies.append(ymport.stl('rod-'+mesh+'.stl',wire=True))

# Spheres
sphereRadius = 0.01
nbSpheres = (32,11,32) 
print "Creating %d spheres..."%(nbSpheres[0]*nbSpheres[1]*nbSpheres[2]),
for i in xrange(nbSpheres[0]):
    for j in xrange(nbSpheres[1]):
		for k in xrange(nbSpheres[2]):
			x = (i*2 - nbSpheres[0])*sphereRadius*1.1+sphereRadius*random.uniform(-0.1,0.1)
			y = -j*sphereRadius*2.2-0.01
			z = (k*2 - nbSpheres[2])*sphereRadius*1.1+sphereRadius*random.uniform(-0.1,0.1)
			r = random.uniform(sphereRadius,sphereRadius*0.9)
			dynamic = True
			color=[0.51,0.52,0.4]
			if (i==0 or i==nbSpheres[0]-1 or j==nbSpheres[1]-1 or k==0 or k==nbSpheres[2]-1):
				dynamic = False
				color=[0.21,0.22,0.1]
			O.bodies.append(utils.sphere([x,y,z],r,color=color,dynamic=dynamic))
print "done\n"

## Estimate time step
#O.dt=utils.PWaveTimeStep()
O.dt=0.0001


## Engines 
O.engines=[
	## Resets forces and momenta the act on bodies
	ForceResetter(),

	## Using bounding boxes find possible body collisions.
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_Facet_Aabb(),
	]),
	InteractionLoop(
		#[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom()],
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		#[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	## Apply gravity
	GravityEngine(gravity=[0,-9.81,0]),
	## Motion equation
	NewtonIntegrator(damping=0.3),
	## Apply kinematics to rod
	TranslationEngine(ids=rod,translationAxis=[0,-1,0],velocity=0.075),
	## Save force on rod
	#ForceRecorder(ids=rod,file='force-'+mesh+'.dat',iterPeriod=50),	
]


import sys,time

print "Start simulation: " + mesh
nbIter=10000

from yade import qt
qt.View()

#for t in xrange(2):
#	start=time.time();O.run(nbIter);O.wait();finish=time.time() 
#	speed=nbIter/(finish-start); print '%g iter/sec\n'%speed
#print "FINISH"
#quit()

