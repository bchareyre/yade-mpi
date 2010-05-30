""" Playground for tuning collider strides depending on maximum velocity. """

from yade import timing,log
import os.path
loadFrom='/tmp/triax.xml'
#if not os.path.exists(loadFrom):
TriaxialTest(numberOfGrains=2000,noFiles=True).generate(loadFrom)
O.load(loadFrom)
log.setLevel('TriaxialCompressionEngine',log.WARN) # shut up
log.setLevel('InsertionSortCollider',log.DEBUG)

collider=utils.typedEngine('InsertionSortCollider')
newton=utils.typedEngine('NewtonIntegrator')

# use striding; say "if 0:" to disable striding and compare to regular runs
if 1:
	# length by which bboxes will be made larger
	collider.sweepLength=.2*O.bodies[100].shape.radius
	# if this is enabled, bboxes will be enlarged based on velocity bin for each body
	if 1:
		collider.nBins=3
		collider.binCoeff=10
		log.setLevel('VelocityBins',log.DEBUG)

O.step() # filter out initialization
O.timingEnabled=True
totalTime=0
# run a few times 500 steps, show timings to see what is the trend
# notably, the percentage of collider time should decrease as the max velocity decreases as well
for i in range(0,5):
	O.run(1000,True)
	timing.stats()
	totalTime+=sum([e.execTime for e in O.engines])
	print 'Number of interactions: %d (real ratio: %g)'%(len(O.interactions),float(O.interactions.countReal())/len(O.interactions))
	print '======================================================='
	timing.reset()

print 'Total time: %g s'%(totalTime/1e9)
#quit()
