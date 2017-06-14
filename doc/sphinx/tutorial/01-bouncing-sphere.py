# basic simulation showing sphere falling ball gravity,
# bouncing against another sphere representing the support

# DATA COMPONENTS

# add 2 particles to the simulation
# they the default material (utils.defaultMat)
O.bodies.append([
	# fixed: particle's position in space will not change (support)
	sphere(center=(0,0,0),radius=.5,fixed=True),
	# this particles is free, subject to dynamics
	sphere((0,0,2),.5)
])

# FUNCTIONAL COMPONENTS

# simulation loop -- see presentation for the explanation
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],        # collision geometry
		[Ip2_FrictMat_FrictMat_FrictPhys()], # collision "physics"
		[Law2_ScGeom_FrictPhys_CundallStrack()]   # contact law -- apply forces
	),
	# Apply gravity force to particles. damping: numerical dissipation of energy.
	NewtonIntegrator(gravity=(0,0,-9.81),damping=0.1)
]

# set timestep to a fraction of the critical timestep
# the fraction is very small, so that the simulation is not too fast
# and the motion can be observed
O.dt=.5e-4*PWaveTimeStep()

# save the simulation, so that it can be reloaded later, for experimentation
O.saveTmp()
