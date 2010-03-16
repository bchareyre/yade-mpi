O.materials.append(FrictMat(young=30e9,poisson=.2,density=4000,frictionAngle=.5))
O.bodies.append([
	utils.sphere((0,0,0),1,dynamic=False),
	utils.sphere((0,0,2.1),1)
])
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_Basic()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(label='newton')
]
O.dt=utils.PWaveTimeStep()

disp=utils.typedEngine('InteractionDispatchers')

disp.callbacks=[SumIntrForcesCb()]
newton.callbacks=[SumBodyForcesCb()]
print O.numThreads
O.step()
