# Deomonstrate composition of KinematicEngine's with +:
# The first engine is composed (from translation and 2 independent rotations, one global and one local)
# The second engine does the same on body #1 as the first one does on #0 (the local rotation is not present)

O.bodies.append([
	utils.sphere((-1,0,0),.3,color=(1,0,.5),dynamic=True,wire=True),
	utils.sphere((1,0,0),.3,color=(0,1,0),dynamic=True,wire=True)
])

O.engines=[
	# only ids of the 1st engine are used
	TranslationEngine(ids=[0],translationAxis=(0,0,1),velocity=.1) + RotationEngine(rotationAxis=(0,0,1),angularVelocity=1,rotateAroundZero=True) + RotationEngine(rotationAxis=(1,0,0),angularVelocity=.3,rotateAroundZero=False),
	HelixEngine(ids=[1],axis=(0,0,1),axisPt=(0,0,0),angularVelocity=1,linearVelocity=.1),
	NewtonIntegrator(warnNoForceReset=False),
]
O.dt=1e-6
O.saveTmp()
