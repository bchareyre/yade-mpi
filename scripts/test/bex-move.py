O.bodies.append(utils.sphere([0,0,0],1,dynamic=True))
O.engines=[
	BexResetter(),
	PeriodicPythonRunner(command='O.bex.addMove(0,(1e-2,0,0))',iterPeriod=1),
	NewtonIntegrator()
]

for i in xrange(0,20):
	O.step()
	print O.bex.f(0),O.bodies[0].state.pos
quit()
