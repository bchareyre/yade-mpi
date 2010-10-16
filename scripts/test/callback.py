O.materials.append(FrictMat(young=30e9,poisson=.2,density=4000,frictionAngle=.5))
O.bodies.append([
	utils.sphere((0,0,0),1,dynamic=False),
	utils.sphere((0,0,2.1),1)
])
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(label='newton')
]
O.dt=utils.PWaveTimeStep()

disp=utils.typedEngine('InteractionLoop')

disp.callbacks=[SumIntrForcesCb()]
newton.callbacks=[SumBodyForcesCb()]
print O.numThreads
O.step()
