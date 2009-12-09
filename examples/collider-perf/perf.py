
utils.readParamsFromTable(nSpheres=8000,collider='InsertionSortCollider',noTableOk=True)
# name of file containing sphere packing with given number of spheres
spheresFile="packing-%dk.spheres"%(nSpheres/1000)

fast='@stride' in collider

import os
if not os.path.exists(spheresFile):
	print "Generating packing"
	p=TriaxialTest(numberOfGrains=nSpheres,radiusMean=1e-3,lowerCorner=[0,0,0],upperCorner=[1,1,1],noFiles=True)
	p.load()
	utils.spheresToFile(spheresFile)
	O.reset()
	print "Packing %s done"%spheresFile
else: print "Packing found (%s), using it."%spheresFile

from yade import timing
O.timingEnabled=True

TriaxialTest(importFilename=spheresFile,fast=fast,noFiles=True).load()
O.dt=utils.PWaveTimeStep()
isc=O.engines[2]
isc['sweepLength']=1e-1

if not fast: utils.replaceCollider(GlobalEngine(collider))

O.step()
timing.stats()
timing.reset()
O.run(200,True)
timing.stats()
quit()
