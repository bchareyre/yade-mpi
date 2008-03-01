# this must be run inside yade
# coding=UTF-8

import os,time,sys
simulFile='/tmp/yade-test-%d.xml'%(os.getpid()) # generated simulations here
broken=['SDECLinkedSpheres','SDECMovingWall','SDECSpheresPlane','ThreePointBending']
genParams={
	'USCTGen':{'spheresFile':'examples/small.sdec.xyz'}
	}
nIter=100
summary=[]
o=Omega()

for pp in o.childClasses('FileGenerator'):
	if pp in broken:
		sys.stderr.write("============= (skipping broken "+pp+")==============\n")
		continue
	sys.stderr.write("============================================= "+pp+" =========================================\n")
	p=Preprocessor(pp)
	if pp in genParams.keys(): # set preprocessor parameters, if desired
		for k in genParams[pp].keys():
			p[k]=genParams[pp][k]
	if not p.generate(simulFile): # preprocessor failed 
		summary.append('FAILED generator '+pp+'!!')
		continue
	o.load(simulFile)
	o.run(10); o.wait() # run first 10 iterations
	start=time.time(); o.run(nIter); o.wait(); finish=time.time() # run nIter iterations, wait to finish, measure elapsed time
	speed=nIter/(finish-start) # rough estimate
	summary.append('Passed test '+pp+' at %g iter/sec'%speed)

for l in summary: print l
