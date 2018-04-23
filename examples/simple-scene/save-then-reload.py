# -*- coding: utf-8 -*-
#  Copyright (C) 2010 by Bruno Chareyre                                  *
#  bruno.chareyre_at_grenoble-inp.fr                                     *

## This short modified version of examples/triax-tutorial/script-session1.py shows how to reload saved states and continue
## without the need to save user variables and functions separately (a typical issue as seen in e.g.
## https://answers.launchpad.net/yade/+question/665314).
## Here the technique is combined with batch execution, getting some parameters from readParamsFromTable(),
## but it is not a requirement.
## Comments on the simulation itself can be found in script-session1.py

nRead=readParamsFromTable(
	num_spheres=1001,
	compFricDegree = 30,
	key='_triax_base_',
	unknownOk=True
)
from yade.params import table

## Define the filename for saved state. By including paramater values in the name
## we make sure that a new state is generated for each parameter set - including a different 'key' name.
initStateFilename="confined_N="+table.key+str(table.num_spheres)+"_fric="+str(table.compFricDegree)+"_.yade.gz"

## Check if a saved state exists and proceed to function/variable declarations as usual.
import os.path
savedState = os.path.exists(initStateFilename)

## user defined variables
num_spheres=table.num_spheres
key=table.key
targetPorosity = 0.43
compFricDegree = table.compFricDegree
finalFricDegree = 30
rate=-0.02
damp=0.2
stabilityThreshold=0.01
young=5e6
mn,mx=Vector3(0,0,0),Vector3(1,1,1)

import time
startTime=time.time()

## user function saving variables, it will not be saved with the simulation; which is ok since it is always re-defined here.
from yade import plot
def history():
  	plot.addData(e11=-triax.strain[0], e22=-triax.strain[1], e33=-triax.strain[2],
  		    ev=-triax.strain[0]-triax.strain[1]-triax.strain[2],
		    s11=-triax.stress(triax.wall_right_id)[0],
		    s22=-triax.stress(triax.wall_top_id)[1],
		    s33=-triax.stress(triax.wall_front_id)[2],
		    i=O.iter)

O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=radians(compFricDegree),density=2600,label='spheres'))
O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=0,density=0,label='walls'))
walls=aabbWalls([mn,mx],thickness=0,material='walls')
wallIds=O.bodies.append(walls)

## the cloud generation is made conditional only because makeCloud() takes time for large num_spheres. Regenerating an initial
## cloud before reloading a saved state would be harmless but useless since the cloud will be replaced by the saved state anyway.
## materials defined hereabove will also be replaced by the the reloaded ones (which are identical) with no real impact on
## initialization time, hence no real need for a condition in that case
from yade import pack
sp=pack.SpherePack()
if not savedState:
	sp.makeCloud(mn,mx,-1,0.3333,num_spheres,False, 0.95,seed=1)
	O.bodies.append([sphere(center,rad,material='spheres') for center,rad in sp])

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8),
	TriaxialStressController(maxMultiplier=1.+2e4/young, finalMaxMultiplier=1.+2e3/young, thickness = 0, stressMask = 7, internalCompaction=True,label="triax"),
	PyRunner(iterPeriod=20,command='history()',label='recorder'),
	NewtonIntegrator(damping=damp,label="newton")
]

triax.goal1=triax.goal2=triax.goal3=-10000

## If no dense state has been generated previously proceed to confinement, else reload
if not savedState:
	print "No saved state - running isotropic confinement for num_spheres=", str(table.num_spheres),", compFricDegree=", str(table.compFricDegree),", key='",str(table.key),"'"
	while 1:
		O.run(1000, True)
		unb=unbalancedForce()
		print 'unbalanced force:',unb,' mean stress: ',triax.meanStress
		if unb<stabilityThreshold and abs(-10000-triax.meanStress)/10000<0.001:
			break
	while triax.porosity>targetPorosity:
		compFricDegree = 0.95*compFricDegree
		setContactFriction(radians(compFricDegree))
		O.run(500,1)
	print "Confinement achieved, save then proceed to deviatoric loading"
	O.save(initStateFilename)
else:
	print "Saved state found - reload then proceed to deviatoric loading for num_spheres=", str(table.num_spheres),", compFricDegree=", str(table.compFricDegree),", key='",str(table.key),"'"
	O.load(initStateFilename)


## Run triaxial simulation starting from a dense  packing
triax.internalCompaction=False
setContactFriction(radians(finalFricDegree))
triax.stressMask = 5
triax.goal2=rate
triax.goal1=-10000
triax.goal3=-10000
newton.damping=0.1

O.run(100,True)

print "Total execution time (savedState=",str(savedState),"): ",str(time.time()-startTime),"s"
