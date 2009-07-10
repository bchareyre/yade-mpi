""" Playground for tuning collider strides depending on maximum velocity. """

from yade import timing,log
import os.path
loadFrom='/tmp/triax.xml'
if not os.path.exists(loadFrom):
	TriaxialTest(numberOfGrains=8000,fast=fast,noFiles=True).generate(loadFrom)
O.load(loadFrom)
log.setLevel('TriaxialCompressionEngine',log.WARN) # shut up

collider=utils.typedEngine('InsertionSortCollider')
newton=utils.typedEngine('NewtonsDampedLaw')

# use striding; say "if 0:" to disable striding and compare to regular runs
if 1:
	collider['stride']=4
	collider['sweepVelocity']=16
	# try to update maximum velocity dynamically:
	if 1:
		O.engines=O.engines+[PeriodicPythonRunner(command='adjustMaxVelocity()',iterPeriod=50)]

def adjustMaxVelocity():
	newMax=sqrt(newton['maxVelocitySq']); newton['maxVelocitySq']=0
	collider['sweepVelocity']=newMax
	# try to adjust stride here; the 4 and 16 are original (reference) values for stride and sweepVelocity
	# if this were inverse-proportional (without sqrt), stride is getting too high
	collider['stride']=4*int(sqrt(16/newMax))
	print 'step %d; new max velocity %g, stride %d'%(O.iter,newMax,collider['stride'])


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

print 'Total time: %g s'%(totalTime/1e9)
quit()
