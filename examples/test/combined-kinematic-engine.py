box = geom.facetBox(center=(0,0,0),extents=(1,2,3), color=(0,1,0))
O.bodies.append(box)
ids = [b.id for b in box]

# set this parameter according to your computer power to make the simulation vizualization slower of faster
coeff = 5.0

nIterPerOneCycle = int(50000/coeff)
vel = 10000*coeff
angVel = 8000*coeff

# function for changing motion, sets transEngine.translationAxis, rotEngine.angularVelocity and rotEngine.zeroPoint
def updateKinematicEngines():
	part = (O.iter / nIterPerOneCycle) % 4
	if   part == 0: # fist part
		v = Vector3(1,0,0)
		av = 0
	elif part == 1: # second part
		v = Vector3(0,1,0)
		av = angVel
	elif part == 2: # third part
		v = Vector3(-1,0,0)
		av = -2*angVel
	elif part == 3: # fourth part
		v = Vector3(0,-1,0)
		av = angVel
	transEngine.translationAxis = v
	rotEngine.angularVelocity = av
	rotEngine.zeroPoint += v*vel*O.dt

O.engines = [
	ForceResetter(),
	PyRunner(iterPeriod=1, command="updateKinematicEngines()" ),
	# construct CombinedKinematicEngine with label, add (with +) TranslationEngine and RotationEngine
	CombinedKinematicEngine(ids=ids,label='combEngine') + TranslationEngine(translationAxis=(1,0,0),velocity=vel) + RotationEngine(rotationAxis=(0,0,1), angularVelocity=0, rotateAroundZero=True, zeroPoint=(0,0,0)),
	NewtonIntegrator(),
]

# get TranslationEngine and RotationEngine from CombinedKinematicEngine
transEngine, rotEngine = combEngine.comb[0], combEngine.comb[1]
print
print 'transEngine:', transEngine
print 'rotEngine:', rotEngine
print


try:
	from yade import qt
	qt.View()
except:
	print 'No graphics, sorry..'

O.run()
