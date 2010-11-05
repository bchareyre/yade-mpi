
# With Thinkpad laptops, control the sphere's motion by tilting the
# laptop.

O.bodies.append(
	utils.facetBox(center=(0,0,.1),extents=(.5,.5,.1),wallMask=31,color=(0,0,1))+
	[utils.sphere((0,0,.1),.04,color=(1,1,1))]
)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop([Ig2_Facet_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
	HdapsGravityEngine(calibrate=(-495,-495),calibrated=True,zeroGravity=(0,0,-1)),
	NewtonIntegrator(damping=.3),
]
O.dt=utils.PWaveTimeStep()

import yade.qt
yade.qt.View()
O.run()
