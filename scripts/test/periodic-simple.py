from yade import log
log.setLevel("PeriodicInsertionSortCollider",log.TRACE)
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),MetaInteractingGeometry2AABB()]),
	PeriodicInsertionSortCollider(label='collider'),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	GravityEngine(gravity=[0,0,-10]),
	NewtonsDampedLaw(damping=.1)
]
O.bodies.append(utils.sphere([0,0,2],1,dynamic=False,density=1000))
O.bodies.append(utils.sphere([0,0,3],1,density=1000))
O.periodicCell=((-5,-5,0),(5,5,10))
O.dt=utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller()
qt.View()
O.run(17)

