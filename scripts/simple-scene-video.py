#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

O.initializers=[BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()])]
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingBox2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(),InteractingBox2InteractingSphere4SpheresContactGeometry()],
		[SimpleElasticRelationships()],
		[ef2_Spheres_Elastic_ElasticLaw()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonsDampedLaw(damping=.2)
]

O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
O.dt=.4*utils.PWaveTimeStep()

from yade import qt
O.stopAtIter=15000
qt.makeSimulationVideo('/tmp/aa.ogg',iterPeriod=100,fps=12)





