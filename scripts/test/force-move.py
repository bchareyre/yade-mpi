O.bodies.append(utils.sphere([0,0,0],1,dynamic=True))
O.engines=[
	ForceResetter(),
	PeriodicPythonRunner(command='O.forces.addMove(0,(1e-2,0,0))',iterPeriod=1),
	NewtonIntegrator()
]

for i in xrange(0,20):
	O.step()
	print O.forces.f(0),O.bodies[0].state.pos
quit()
