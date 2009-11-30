#! file to test BasicPM.cpp
# -*- coding: utf-8 -*-

#!/usr/local/bin/yade-trunk -x
# -*- coding: utf-8 -*-
# -*- encoding=utf-8 -*-


o=Omega()
o.initializers=[
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),MetaInteractingGeometry2AABB()])
]

o.engines=[
	
	BexResetter(),
	
	BoundingVolumeMetaEngine([
		InteractingSphere2AABB(),
		MetaInteractingGeometry2AABB()
	]),
	
	InsertionSortCollider(),
	
	InteractionGeometryMetaEngine([
		ef2_Sphere_Sphere_Dem3DofGeom()
	]),
	
	InteractionPhysicsMetaEngine([Ip2_CSMat_CSMat_CSPhys()]),
	
	CundallStrackLaw(),

	
	GravityEngine(gravity=[0,0,-9.81]),
	
	
	
	PhysicalActionApplier([
		NewtonsForceLaw(),
		NewtonsMomentumLaw(),
	]),

	PhysicalParametersMetaEngine([LeapFrogPositionIntegrator()]),

	PhysicalParametersMetaEngine([LeapFrogOrientationIntegrator()])

]


from yade import utils

for b in o.bodies:
  b.mask = 1

o.bodies.append(utils.sphere([0,0,2],1,dynamic = False, color=[0,1,0],young=30e9,poisson=.3,density=2400))
o.bodies.append(utils.sphere([0,0,6],1,color=[0,0,1],young=30e9,poisson=.3,density=2400))

o.dt=.2*utils.PWaveTimeStep()


from yade import qt
qt.Controller()
qt.View()

 
