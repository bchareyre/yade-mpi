O.materials.append(FrictMat(young=30e9,poisson=.2,density=4000,frictionAngle=.5))
O.bodies.append([
	sphere((0,0,0),1,fixed=True),
	sphere((0,0,2.1),1)
])
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(label='newton',gravity=(0,0,-9.81))
]
O.dt=PWaveTimeStep()

disp=typedEngine('InteractionLoop')

disp.callbacks=[SumIntrForcesCb()]
#newton.callbacks=[SumBodyForcesCb()]
print O.numThreads
O.step()
