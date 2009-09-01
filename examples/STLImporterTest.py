#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
tc = 0.001
en = 0.3
es = 0.3

## Import wall's geometry
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
imported = utils.import_stl_geometry('baraban.stl',frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",**params) # **params sets kn, cn, ks, cs

## Spheres
sphereRadius = 0.2
nbSpheres = (10,10,10)
#nbSpheres = (5,5,5)
for i in xrange(nbSpheres[0]):
    for j in xrange(nbSpheres[1]):
        for k in xrange(nbSpheres[2]):
            x = (i*2 - nbSpheres[0])*sphereRadius*1.1
            y = j*sphereRadius*2.2
            z = (k*2 - nbSpheres[2])*sphereRadius*1.1
            s=utils.sphere([x,y,z],sphereRadius,density=Density,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters")
            p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
            s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']
            O.bodies.append(s)

## Timestep 
O.dt=.2*tc

## Initializers 
O.initializers=[
	## Create bounding boxes. They are needed to zoom the 3d view properly before we start the simulation.
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()])
	]

## Engines 
O.engines=[
	## Resets forces and momenta the act on bodies
	BexResetter(),
	## Associates bounding volume to each body.
	BoundingVolumeMetaEngine([
		InteractingSphere2AABB(),
		InteractingFacet2AABB(),
		MetaInteractingGeometry2AABB()
	]),
	## Using bounding boxes find possible body collisions.
	InsertionSortCollider(),
	## Create geometry information about each potential collision.
	InteractionGeometryMetaEngine([
		InteractingSphere2InteractingSphere4SpheresContactGeometry(),
		InteractingFacet2InteractingSphere4SpheresContactGeometry()
	]),
	## Create physical information about the interaction.
	InteractionPhysicsMetaEngine([SimpleViscoelasticRelationships()]),
    ## Constitutive law
	ConstitutiveLawDispatcher([ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw()]),
	## Apply gravity
	GravityEngine(gravity=[0,-9.81,0]),
	## Cundall damping must been disabled!
	NewtonsDampedLaw(damping=0),
	## Apply kinematics to walls
	RotationEngine(subscribedBodies=imported,rotationAxis=[0,0,1],rotateAroundZero=True,angularVelocity=0.5)
]

from yade import qt
#qt.Controller()
qt.View()
O.run()

