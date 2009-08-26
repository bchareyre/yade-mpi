# -*- encoding=utf-8 -*-

from yade import utils

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
O.dt=0.0001

## Initializers 
O.initializers=[
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()])
	]

## Engines 
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	## Spheres factory engine
	SpheresFactory(factoryFacets=factory1,virtPeriod=0.005,radius=0.07,radiusRange=0.03,young=Young,color=(1,0,0)),
	SpheresFactory(factoryFacets=factory2,virtPeriod= 0.01,radius=0.08,young=Young,color=(0,1,0)),
	SpheresFactory(factoryFacets=factory3,virtPeriod= 0.01,radius=0.05,young=Young,color=(0,0,1)),

	InteractionGeometryMetaEngine([ef2_Facet_Sphere_Dem3DofGeom(),ef2_Sphere_Sphere_Dem3DofGeom()]),
	InteractionPhysicsMetaEngine([SimpleElasticRelationships()]),
	ConstitutiveLawDispatcher([Law2_Dem3Dof_Elastic_Elastic()]),

	GravityEngine(gravity=(0,0,-9.81)),
	NewtonsDampedLaw(damping=0.3)
]
O.saveTmp()

