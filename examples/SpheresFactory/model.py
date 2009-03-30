# -*- encoding=utf-8 -*-

from yade import utils

## Omega
o=Omega() 

## PhysicalParameters 
Young = 0.15e9
Poisson = 0.3

## Import box geometry
box = utils.import_stl_geometry('plane.stl',young=Young,poisson=Poisson,color=[0.7,0.7,0.7],wire=False)
## Import factory geometry
factory1 = utils.import_stl_geometry('yade.stl',color=[0.7,0.4,0.4],noInteractingGeometry=True)
factory2 = utils.import_stl_geometry('circle.stl',color=[0.4,0.7,0.4],noInteractingGeometry=True)
factory3 = utils.import_stl_geometry('square.stl',color=[0.4,0.4,0.7],noInteractingGeometry=True)

## Timestep 
o.dt=0.0001

## Initializers 
o.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',
		[EngineUnit('InteractingSphere2AABB'),
			EngineUnit('InteractingFacet2AABB'),
			EngineUnit('MetaInteractingGeometry2AABB')]),
	]

## Engines 
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingFacet2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider'),

	## Spheres factory engine
	StandAloneEngine('SpheresFactory',{'factoryFacets':factory1,'virtPeriod':0.005,'radius':0.07,'radiusRange':0.03,'young':Young,'color':[1,0,0]}),
	StandAloneEngine('SpheresFactory',{'factoryFacets':factory2,'virtPeriod':0.01,'radius':0.08,'young':Young,'color':[0,1,0]}),
	StandAloneEngine('SpheresFactory',{'factoryFacets':factory3,'virtPeriod':0.01,'radius':0.05,'young':Young,'color':[0,0,1]}),

	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('MacroMicroElasticRelationships')]),

	DeusExMachina('GravityEngine',{'gravity':[0,0,-9.81]}),
	
	StandAloneEngine('ElasticContactLaw'),
	
	MetaEngine('PhysicalActionDamper',[
		EngineUnit('CundallNonViscousForceDamping',{'damping':0.3}),
		EngineUnit('CundallNonViscousMomentumDamping',{'damping':0.3})
	]),

	MetaEngine('PhysicalActionApplier',[
		EngineUnit('NewtonsForceLaw'),
		EngineUnit('NewtonsMomentumLaw'),
	]),

	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogPositionIntegrator')]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogOrientationIntegrator')]),

]

## Save the scene to file, so that it can be loaded later. Supported extension are: .xml, .xml.gz, .xml.bz2.
o.save('/tmp/a.xml');

