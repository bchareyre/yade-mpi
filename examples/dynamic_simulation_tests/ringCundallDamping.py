#!/usr/bin/python
# -*- coding: utf-8 -*-

from yade import utils
from yade import ymport

## Omega
o=Omega() 

## PhysicalParameters 
#Density=500
#frictionAngle=radians(30)
sphereRadius=0.05

## Import wall's geometry
walls = O.bodies.append(ymport.stl('ring.stl'))

def fill_cylinder_with_spheres(sphereRadius,cylinderRadius,cylinderHeight,cylinderOrigin,cylinderSlope):
	spheresCount=0
	for h in xrange(0,cylinderHeight/sphereRadius/2):
			for r in xrange(1,cylinderRadius/sphereRadius/2):
				dfi = asin(0.5/r)*2
				for a in xrange(0,6.28/dfi):
					x = cylinderOrigin[0]+2*r*sphereRadius*cos(dfi*a)
					y = cylinderOrigin[1]+2*r*sphereRadius*sin(dfi*a)
					z = cylinderOrigin[2]+h*2*sphereRadius
					o.bodies.append(utils.sphere([x,y*cos(cylinderSlope)+z*sin(cylinderSlope),z*cos(cylinderSlope)-y*sin(cylinderSlope)],sphereRadius))
					spheresCount+=1
	return spheresCount

## Spheres
spheresCount=0
spheresCount+=fill_cylinder_with_spheres(sphereRadius,0.5,0.10,[0,0,0],radians(0))
print "Number of spheres: %d" % spheresCount

## Initializers 
o.initializers=[
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),])
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

	## NOTE: Non zero Cundall damping affected a dynamic simulation!
	NewtonIntegrator(damping=0.3),

	## Apply kinematics to walls
    ## angularVelocity = 0.73 rad/sec = 7 rpm
	RotationEngine(subscribedBodies=walls,rotationAxis=[0,0,1],rotateAroundZero=True,angularVelocity=0.73)
]

for b in o.bodies:
    if b.shape.name=='Sphere':
        b.state.blockedDOFs=['z'] # blocked movement along Z

o.dt=0.02*utils.PWaveTimeStep()

o.saveTmp('init');

from yade import qt
renderer=qt.Renderer()
renderer.wire=True
#qt.Controller()
qt.View()
O.run()

