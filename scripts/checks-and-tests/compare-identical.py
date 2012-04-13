"""
This files tests simulation correctness by loading a simulation two times,
and periodically comparing the results, showing diffs in the simulation XML,
if any. It stops after the differece is first spotted.

On openMP-enabled installs, it should be run with OMP_NUM_THREADS=1, otherwise
there will be noise differences coming from non-deterministic order of interactions
in the container (although they are the same). You can use OMP_NUM_THREADS=2 to see
what happens if the simulations get different.
"""
# this is to provide some default simulation to test on
# comment it out and provide your own simulation XML in init
TriaxialTest().generate('/tmp/TriaxialTest.xml')
#

# what is the initial file to load
initFile='/tmp/TriaxialTest.xml'
# that is the prefix for HTML diffs, if any
outPrefix='/tmp/scene_'
# at which step to stop
stopIter=2000
# how may steps to run between comparisons
nSteps=100

# quiet annoying messages

if O.numThreads>1:
	print "WARNING: You should run single-threaded with OMP_NUM_THREADS=1; interaction order will be probably different otherwise!"

for scene in 0,1:
	O.load(initFile); O.interactions.serializeSorted=True; O.switchScene();
from hashlib import md5; import difflib,sys
print "Identical at steps ",
for i in xrange(0,stopIter/nSteps):
	sys.stdout.flush()
	for scene in 'A','B':
		O.run(nSteps,True); O.saveTmp(scene); O.switchScene()
	A,B=O.tmpToString('A'),O.tmpToString('B')
	# fast compare first using hash digest
	Ahash,Bhash=md5(A),md5(B)
	if Ahash.digest()==Bhash.digest():
		print O.iter,; continue
	print "\nComputing differences..."
	diff=difflib.HtmlDiff(tabsize=3,wrapcolumn=80)
	outName=outPrefix+'%05d_diff.html'%O.iter
	out=open(outName,'w')
	out.write(diff.make_file(A.split('\n'),B.split('\n'),context=True,numlines=2))
	print 'file://%s'%outName
	break # stop at the first different value
	
