#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

from yade import utils
import random

## PhysicalParameters 
Young = 15e6
Poisson = 0.2

## Variant of mesh
mesh = 'coarse'
#mesh = 'fine'
#mesh = 'tiny'

## Omega
o=Omega() 

## Import geometry 
rod = utils.import_stl_geometry('rod-'+mesh+'.stl',wire=True,young=Young,poisson=Poisson)
shrinkFactor=0.005

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
			o.bodies.append(utils.sphere([x,y,z],r,young=Young,poisson=Poisson,density=2400,color=color,dynamic=dynamic))
print "done\n"

## Estimate time step
#o.dt=utils.PWaveTimeStep()
o.dt=0.0001

## Initializers 
o.initializers=[
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()])
	]

## Engines 
o.engines=[
	## Resets forces and momenta the act on bodies
	BexResetter(),

	## Associates bounding volume to each body.
	BoundDispatcher([
		InteractingSphere2AABB(),
		InteractingFacet2AABB(),
		MetaInteractingGeometry2AABB()
	]),
	## Using bounding boxes find possible body collisions.
	InsertionSortCollider(),
	## Create geometry information about each potential collision.
	InteractionGeometryDispatcher([
		InteractingSphere2InteractingSphere4SpheresContactGeometry(),
		InteractingFacet2InteractingSphere4SpheresContactGeometry()
	]),
	## Create physical information about the interaction.
	InteractionPhysicsDispatcher([MacroMicroElasticRelationships()]),
    ## Constitutive law
	ElasticContactLaw(),
	## Apply gravity
	GravityEngine(gravity=[0,-9.81,0]),
	## Motion equation
	NewtonsDampedLaw(damping=0.3),
	## Apply kinematics to rod
	TranslationEngine(subscribedBodies=rod,translationAxis=[0,-1,0],velocity=0.075),
	## Save force on rod
	ForceRecorder(startId=0,endId=len(rod)-1,outputFile='force-'+mesh+'.dat',interval=50),	
	## Save positions
	SQLiteRecorder(recorders=['se3'],dbFile='positions-'+mesh+'.sqlite',iterPeriod=100)

]

o.save('/tmp/scene.xml.bz2');

import sys,time

print "Start simulation: " + mesh
nbIter=10000
for t in xrange(2):
	start=time.time();o.run(nbIter);o.wait();finish=time.time() 
	speed=nbIter/(finish-start); print '%g iter/sec\n'%speed
print "FINISH"
quit()

