#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

from yade import utils
import random
from yade import ymport

## PhysicalParameters 

## Variant of mesh
mesh = 'coarse'
#mesh = 'fine'
#mesh = 'tiny'

## Omega
o=Omega() 

## Import geometry 
#rod = O.bodies.append(ymport.stl('rod-'+mesh+'.stl',wire=True))

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
			o.bodies.append(utils.sphere([x,y,z],r,color=color,dynamic=dynamic))
print "done\n"

## Estimate time step
#o.dt=utils.PWaveTimeStep()
o.dt=0.0001

## Initializers 
o.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()])
	]

## Engines 
o.engines=[
	## Resets forces and momenta the act on bodies
	ForceResetter(),

	## Associates bounding volume to each body.
	BoundDispatcher([
		Bo1_Sphere_Aabb(),
		Bo1_Facet_Aabb(),
	]),
	## Using bounding boxes find possible body collisions.
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_Basic()],
	),
	## Apply gravity
	GravityEngine(gravity=[0,-9.81,0]),
	## Motion equation
	NewtonIntegrator(damping=0.3),
	## Apply kinematics to rod
	TranslationEngine(subscribedBodies=rod,translationAxis=[0,-1,0],velocity=0.075),
	## Save force on rod
	ForceRecorder(subscribedBodies=rod,file='force-'+mesh+'.dat',iterPeriod=50),	
]


import sys,time

print "Start simulation: " + mesh
nbIter=10000
for t in xrange(2):
	start=time.time();o.run(nbIter);o.wait();finish=time.time() 
	speed=nbIter/(finish-start); print '%g iter/sec\n'%speed
print "FINISH"
quit()

