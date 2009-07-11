""" Playground for tuning collider strides depending on maximum velocity. """

from yade import timing,log
import os.path
loadFrom='/tmp/triax.xml'
if not os.path.exists(loadFrom):
	TriaxialTest(numberOfGrains=8000,fast=True,noFiles=True).load()
	#O.run(500)
	O.save(loadFrom)
O.load(loadFrom)
log.setLevel('TriaxialCompressionEngine',log.WARN) # shut up
log.setLevel('InsertionSortCollider',log.DEBUG)

collider=utils.typedEngine('InsertionSortCollider')
newton=utils.typedEngine('NewtonsDampedLaw')

# use striding; say "if 0:" to disable striding and compare to regular runs
if 0:
	# length by which bboxes will be made larger
	collider['sweepLength']=0.05*O.bodies[100].shape['radius']

O.step() # filter out initialization
O.timingEnabled=True
totalTime=0
# run a few times 500 steps, show timings to see what is the trend
# notably, the percentage of collider time should decrease as the max velocity decreases as well
for i in range(0,5):
	O.run(500,True)
	timing.stats()
	totalTime+=sum([e.execTime for e in O.engines])
	print 'Number of interactions: %d (real ratio: %g)'%(len(O.interactions),float(O.interactions.countReal())/len(O.interactions))
	print '======================================================='
	timing.reset()
	# plot velocity histogram
	if 0:
		import pylab
		pylab.hist([sqrt(sum([v**2 for v in b.phys['velocity']])) for b in O.bodies])
		pylab.title('step %d; max speed %g'%(O.iter,sqrt(newton['maxVelocitySq'])))
		pylab.grid()
		pylab.savefig('%s-%05d.png'%(loadFrom,O.iter),dpi=40)
		pylab.cla()

print 'Total time: %g s'%(totalTime/1e9)
#quit()
