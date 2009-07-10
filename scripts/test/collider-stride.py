
O.bodies.append([
	utils.facet([[1,0,0],[-1,2,0],[-1,-2,0]]),
	utils.sphere([0,0,2],.5,color=(0,1,0),density=2e3,velocity=[0,0,-1])
])
O.dt=utils.PWaveTimeStep()

O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(label='isc'),
	InteractionDispatchers(
		[ef2_Facet_Sphere_Dem3DofGeom(),ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	GravityEngine(gravity=[0,0,-100]),
	NewtonsDampedLaw(damping=0)
]

from yade import timing,qt,log
try:
	renderer=qt.Renderer()
	renderer['Body_bounding_volume']=True
except ImportError: pass

O.timingEnabled=True
isc['stride']=3
isc['sweepDistSafetyFactor']=.1
isc['sweepTimeSafetyFactor']=.1
#log.setLevel('InsertionSortCollider',log.DEBUG)
#log.setLevel('BoundingVolumeMetaEngine',log.DEBUG)
O.saveTmp()
#O.run(10000,True)
#timing.stats()
O.step()
