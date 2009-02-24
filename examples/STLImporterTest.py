#!/usr/local/bin/yade-trunk -x
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
p=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
imported = utils.import_stl_geometry('baraban.stl',frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",physParamsAttr={'kn':p['kn'],'cn':p['cn'],'ks':p['ks'],'cs':p['cs']})

## Spheres
sphereRadius = 0.2
#nbSpheres = (10,10,10)
nbSpheres = (5,5,5)
for i in xrange(nbSpheres[0]):
    for j in xrange(nbSpheres[1]):
        for k in xrange(nbSpheres[2]):
            x = (i*2 - nbSpheres[0])*sphereRadius*1.1
            y = j*sphereRadius*2.2
            z = (k*2 - nbSpheres[2])*sphereRadius*1.1
            s=utils.sphere([x,y,z],sphereRadius,density=Density,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters")
            p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
            s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']
            o.bodies.append(s)

## Timestep 
o.dt=.2*tc

## Initializers 
o.initializers=[
	## Create and reset to zero container of all PhysicalActions that will be used
	StandAloneEngine('PhysicalActionContainerInitializer'),
	## Create bounding boxes. They are needed to zoom the 3d view properly before we start the simulation.
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingFacet2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]

## Engines 
o.engines=[
	## Resets forces and momenta the act on bodies
	StandAloneEngine('PhysicalActionContainerReseter'),
	## Associates bounding volume to each body.
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingFacet2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	## Using bounding boxes find possible body collisions.
    StandAloneEngine('PersistentSAPCollider'),
	## Create geometry information about each potential collision.
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry')
	]),
	## Create physical information about the interaction.
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleViscoelasticRelationships')]),
    ## Constitutive law
	MetaEngine('ConstitutiveLawDispatcher',[EngineUnit('Spheres_Viscoelastic_SimpleViscoelasticContactLaw')]),
	## Apply gravity
	DeusExMachina('GravityEngine',{'gravity':[0,-9.81,0]}),
	## Cundall damping must been disabled!
	DeusExMachina('NewtonsDampedLaw',{'damping':0}),
	## Apply kinematics to walls
	DeusExMachina('RotationEngine',{'subscribedBodies':imported,'rotationAxis':[0,0,1],'rotateAroundZero':True,'angularVelocity':0.5}),
]

o.saveTmp('init');

