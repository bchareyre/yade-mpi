# -*- encoding=utf-8 -*-

from yade import utils

## Omega
o=Omega() 

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
sphereRadius=0.05
tc = 0.001
en = 0.3
es = 0.3

## Import wall's geometry
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
walls = utils.import_stl_geometry('ring.stl',frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",**params)

def fill_cylinder_with_spheres(sphereRadius,cylinderRadius,cylinderHeight,cylinderOrigin,cylinderSlope):
	spheresCount=0
	for h in xrange(0,cylinderHeight/sphereRadius/2):
			for r in xrange(1,cylinderRadius/sphereRadius/2):
				dfi = asin(0.5/r)*2
				for a in xrange(0,6.28/dfi):
					x = cylinderOrigin[0]+2*r*sphereRadius*cos(dfi*a)
					y = cylinderOrigin[1]+2*r*sphereRadius*sin(dfi*a)
					z = cylinderOrigin[2]+h*2*sphereRadius
					s=utils.sphere([x,y*cos(cylinderSlope)+z*sin(cylinderSlope),z*cos(cylinderSlope)-y*sin(cylinderSlope)],sphereRadius,density=Density,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters")
					p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
					s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']
					o.bodies.append(s)
					spheresCount+=1
	return spheresCount

# Spheres
spheresCount=0
spheresCount+=fill_cylinder_with_spheres(sphereRadius,0.5,0.10,[0,0,0],radians(0))
print "Number of spheres: %d" % spheresCount

## Initializers 
o.initializers=[
	## Create bounding boxes. They are needed to zoom the 3d view properly before we start the simulation.
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB()])
	]

## Engines 
o.engines=[
	## Resets forces and momenta the act on bodies
	BexResetter(),

	## Associates bounding volume to each body.
	BoundDispatcher([
		InteractingSphere2AABB(),
		InteractingFacet2AABB(),
	]),

	## Using bounding boxes find possible body collisions.
	InsertionSortCollider(),

	## Create geometry information about each potential collision.
	InteractionGeometryDispatcher([
		Ig2_Sphere_Sphere_ScGeom(),
		Ig2_Facet_Sphere_ScGeom()
	]),

	## Create physical information about the interaction.
	InteractionPhysicsDispatcher([SimpleViscoelasticRelationships()]),

    ## Constitutive law
	ConstitutiveLawDispatcher([ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw()]),

	## Apply gravity
	GravityEngine(gravity=[0,-9.81,0]),
	## Cundall damping must been disabled!
	NewtonIntegrator(damping=0),
	## Apply kinematics to walls
    ## angularVelocity = 0.73 rad/sec = 7 rpm
	RotationEngine(subscribedBodies=walls,rotationAxis=[0,0,1],rotateAroundZero=True,angularVelocity=0.73)

]

for b in o.bodies:
    if b.shape.name=='Sphere':
        b.phys.blockedDOFs=['z']

o.dt=0.2*tc

o.saveTmp('init');

from yade import qt
renderer=qt.Renderer()
renderer['Body_wire']=True
#qt.Controller()
qt.View()
O.run()

