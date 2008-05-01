#!/usr/local/bin/yade-trunk
# coding=UTF-8
# this must be run inside yade

import os,time,sys
simulFile='/tmp/yade-test-%d.xml'%(os.getpid()) # generated simulations here
pyCmdFile='/tmp/yade-test-%d.py'%(os.getpid()) # generated simulations here
speedFile='/tmp/yade-test-%d.speed'%(os.getpid()) # write speed hiere
runSimul="""
# generated file
simulFile='%s'
speedFile='%s'
nIter=%d
import time
o=Omega()
o.load(simulFile)
o.run(10); o.wait() # run first 10 iterations
start=time.time(); o.run(nIter); o.wait(); finish=time.time() # run nIter iterations, wait to finish, measure elapsed time
speed=nIter/(finish-start) # rough estimate
open(speedFile,'w').write('%%g'%%speed)
print quit
quit()
"""%(simulFile,speedFile,100)

f=open(pyCmdFile,'w'); f.write(runSimul); f.close()

#broken=['SDECLinkedSpheres','SDECMovingWall','SDECSpheresPlane','ThreePointBending']
genParams={
	#'USCTGen':{'spheresFile':'examples/small.sdec.xyz'}
	}
#nIter=100
summary=[]
o=Omega()
broken=[]

def crashProofRun():
	import os
	import yade.runtime
	retval=os.system("%s -N PythonUI -- -s '%s'"%(yade.runtime.executable,pyCmdFile))
	if retval==0: return 'passed (%s iter/sec)'%(open(speedFile,'r').readline()[:-1])
	else: return 'CRASHED (backtrace above)'

generators=o.childClasses('FileGenerator')

for pp in o.childClasses('FileGenerator'):
	if pp in broken:
		print "============= (skipping broken "+pp+")=============="
		summary.append("%30s SKIPPED (broken)"%pp)
		continue
	print "============================================= "+pp+" =========================================\n"
	p=Preprocessor(pp)
	if pp in genParams.keys(): # set preprocessor parameters, if desired
		for k in genParams[pp].keys():
			p[k]=genParams[pp][k]
	if not p.generate(simulFile): # preprocessor failed 
		summary.append('%30s FAILED generator'%pp)
		continue
	if 1: # survives crash (separate process)
		summary.append('%30s '%pp+crashProofRun())
	else:
		o.load(simulFile)
		o.run(10); o.wait() # run first 10 iterations
		start=time.time(); o.run(nIter); o.wait(); finish=time.time() # run nIter iterations, wait to finish, measure elapsed time
		speed=nIter/(finish-start) # rough estimate
		summary.append('%30s passed (%g iter/sec)'%(pp,speed))

print "\n\n========================================= SUMMARY ======================================\n"
for l in summary: print l
print "\n====================================== END OF SUMMARY =================================="
import sys
sys.exit(0) # quit() is not in python2.4
