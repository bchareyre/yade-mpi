#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

## Open STL file
imp = STLImporter()
imp.import_vertices = True
imp.import_edges = True
imp.import_facets = True
imp.facets_wire = True
imp.open('baraban.stl')

## Omega
o=Omega() 

## Initializers 
o.initializers=[
	## Create and reset to zero container of all PhysicalActions that will be used
	StandAloneEngine('PhysicalActionContainerInitializer'),
	## Create bounding boxes. They are needed to zoom the 3d view properly before we start the simulation.
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingEdge2AABB'),EngineUnit('InteractingFacet2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]

## Engines 
o.engines=[
	## Resets forces and momenta the act on bodies
	StandAloneEngine('PhysicalActionContainerReseter'),
	## Associates bounding volume to each body.
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingEdge2AABB'),
		EngineUnit('InteractingFacet2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	## Using bounding boxes find possible body collisions.
	StandAloneEngine('SpatialQuickSortCollider'),
	## Create geometry information about each potential collision.
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingVertex2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingEdge2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry')
	]),
	## Create physical information about the interaction.
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('MacroMicroElasticRelationships')]),
	## "Solver" of the contact
	StandAloneEngine('ElasticContactLaw'),
	## Apply gravity
	DeusExMachina('GravityEngine',{'gravity':[0,-9.81,0]}),
	## Forces acting on bodies are damped to artificially increase energy dissipation in simulation.
	MetaEngine('PhysicalActionDamper',[
		EngineUnit('CundallNonViscousForceDamping',{'damping':0.3}),
		EngineUnit('CundallNonViscousMomentumDamping',{'damping':0.3})
	]),
	## Now we have forces and momenta acting on bodies. Newton's law calculates acceleration that corresponds to them.
	MetaEngine('PhysicalActionApplier',[
		EngineUnit('NewtonsForceLaw'),
		EngineUnit('NewtonsMomentumLaw'),
	]),
	## Acceleration results in velocity change. Integrating the velocity over dt, position of the body will change.
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogPositionIntegrator')]),
	## Angular acceleration changes angular velocity, resulting in position and/or orientation change of the body.
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogOrientationIntegrator')]),
	## Apply kinematics to walls
	DeusExMachina('RotationEngine',{'subscribedBodies':range(imp.number_of_all_imported),'rotationAxis':[0,0,1],'rotateAroundZero':True,'angularVelocity':0.5}),
]

from yade import utils

## PhysicalParameters 
Young = 15e6
Poisson = 0.2

## Import walls geometry from STL file
for i in xrange(imp.number_of_all_imported):
    b=Body()
    b['isDynamic']=False
    b.phys=PhysicalParameters('BodyMacroParameters',{'se3':[0,0,0,1,0,0,0],'mass':0,'inertia':[0,0,0],'young':Young,'poisson':Poisson})
    if(i>=imp.number_of_imported_vertices):
	b.bound=BoundingVolume('AABB',{'diffuseColor':[0,1,0]})
    o.bodies.append(b)
imp.import_geometry(o.bodies)

## Spheres
sphereRadius = 0.2
nbSpheres = (10,10,10)
for i in xrange(nbSpheres[0]):
    for j in xrange(nbSpheres[1]):
	for k in xrange(nbSpheres[2]):
	    x = (i*2 - nbSpheres[0])*sphereRadius*1.1
	    y = j*sphereRadius*2.2
	    z = (k*2 - nbSpheres[2])*sphereRadius*1.1
	    o.bodies.append(utils.sphere([x,y,z],sphereRadius,young=Young,poisson=Poisson,density=2400))
utils.randomizeColors(onlyDynamic=True)

## Timestep 
o.dt=0.001

## Save the scene to file, so that it can be loaded later. Supported extension are: .xml, .xml.gz, .xml.bz2.
o.save('/tmp/a.xml');

utils.runInQtGui() # will run in background
quit()

