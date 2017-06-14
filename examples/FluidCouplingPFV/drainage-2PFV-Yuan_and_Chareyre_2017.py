# encoding: utf-8
# This script demonstrates a simple case of drainage simulation using the "2PFV" two-phase model implemented in UnsaturatedEngine.
# The script was used to generate the result and supplementary material (video) of [1]. The only difference is the problem size (40k particles in the paper vs. 1k (default) in this version)
# [1] Yuan, C., & Chareyre, B. (2017). A pore-scale method for hydromechanical coupling in deformable granular media. Computer Methods in Applied Mechanics and Engineering, 318, 1066-1079. (http://www.sciencedirect.com/science/article/pii/S0045782516307216)

import matplotlib; matplotlib.rc('axes',grid=True)
from yade import pack
import pylab
from numpy import *

utils.readParamsFromTable(seed=1,num_spheres=1000,compFricDegree = 15.0)
from yade.params import table

seed=table.seed
num_spheres=table.num_spheres# number of spheres
compFricDegree = table.compFricDegree # initial contact friction during the confining phase (will be decreased during the REFD compaction process)
confiningS=-1e5

## creat a packing with a specific particle side distribution (PSD)
psdSizes,psdCumm=[.599,0.6,0.849,0.85,1.0,1.40],[0.,0.35,0.35,0.70,.70,1.]
sp=pack.SpherePack()
mn,mx=Vector3(0,0,0),Vector3(10,10,10)
sp.makeCloud(minCorner=mn,maxCorner=mx,psdSizes=psdSizes,psdCumm=psdCumm,distributeMass=True,num=num_spheres,seed=seed)

## create material #0, which will be used as default
O.materials.append(FrictMat(young=15e7,poisson=.4,frictionAngle=radians(compFricDegree),density=2600,label='spheres'))
O.materials.append(FrictMat(young=15e7,poisson=.4,frictionAngle=0,density=0,label='frictionless'))

## create walls around the packing
walls=aabbWalls((mn,mx),thickness=0,material='frictionless')
wallIds=O.bodies.append(walls)

O.bodies.append([utils.sphere(center,rad,material='spheres') for center,rad in sp])

triax=TriaxialStressController(
	internalCompaction=True,
	goal1=confiningS,
	goal2=confiningS,
	goal3=confiningS,
	max_vel=10,
	label="triax"
)

newton=NewtonIntegrator(damping=0.4)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8),
	triax,
	newton
]

while 1:
  O.run(1000,True)
  unb=unbalancedForce()
  if unb<0.01 and abs(triax.goal1-triax.meanStress)/abs(triax.goal1)<0.001:
    break

#############################
##   REACH NEW EQU. STATE ###
#############################
finalFricDegree = 30 # contact friction during the deviatoric loading

#We move to deviatoric loading, let us turn internal compaction off to keep particles sizes constant
triax.internalCompaction=False
# Change contact friction (remember that decreasing it would generate instantaneous instabilities)
setContactFriction(radians(finalFricDegree))

while 1:
  O.run(1000,True)
  unb=unbalancedForce()
  if unb<0.001 and abs(triax.goal1-triax.meanStress)/abs(triax.goal1)<0.001:
    break  

triax.depth0=triax.depth
triax.height0=triax.height
triax.width0=triax.width
O.save('1kPacking.yade') #save the packing, which can be reloaded later.

O.run(1000,True)
ei0=-triax.strain[0];ei1=-triax.strain[1];ei2=-triax.strain[2]
si0=-triax.stress(0)[0];si1=-triax.stress(2)[1];si2=-triax.stress(4)[2]

from yade import plot
O.engines=O.engines+[PyRunner(iterPeriod=20,command='history()',dead=1,label='recorder')]

def history():
  	plot.addData(e11=-triax.strain[0]-ei0, e22=-triax.strain[1]-ei1, e33=-triax.strain[2]-ei2,
		    s11=-triax.stress(0)[0]-si0,
		    s22=-triax.stress(2)[1]-si1,
		    s33=-triax.stress(4)[2]-si2,
		    pc=-unsat.bndCondValue[2],
		    sw=unsat.getSaturation(False),
		    i=O.iter
		    )

plot.plots={'pc':('sw',None,'e22')}
plot.plot()

#######################################################
##     Drainage Test under oedometer conditions     ###
#######################################################
##oedometer conditions
triax.stressMask=2
triax.goal1=triax.goal3=0
goalTop=triax.stress(3)[1]
triax.goal2=goalTop
triax.wall_bottom_activated=0
recorder.dead=0

##Instantiate a two-phase engine
unsat=UnsaturatedEngine()
meanDiameter=(O.bodies[-1].shape.radius + O.bodies[6].shape.radius) / 2.

##set boundary conditions, the drainage is controlled by decreasing W-phase pressure and keeping NW-phase pressure constant
unsat.bndCondIsPressure=[0,0,1,1,0,0]
unsat.bndCondValue=[0,0,-1e8,0,0,0]
unsat.isPhaseTrapped=True #the W-phase can be disconnected from its reservoir
unsat.initialization()
unsat.surfaceTension = 10

##start invasion, the data of normalized pc-sw-strain will be written into pcSwStrain.txt
file=open('pcSwStrain.txt',"w")
for pg in arange(1.0e-5,15.0,0.1):
  #increase gaz pressure at the top boundary
  unsat.bndCondValue=[0,0,(-1.0)*pg*unsat.surfaceTension/meanDiameter,0,0,0]
  #compute the evolution of interfaces
  unsat.invasion()
  #save the phases distribution in vtk format, to be displayed by paraview
  unsat.savePhaseVtk("./")
  #compute and apply the capillary forces on each particle
  unsat.computeCapillaryForce()
  for b in O.bodies:
    O.forces.setPermF(b.id, unsat.fluidForce(b.id))
  #reac
  while 1:
    O.run(1000,True)
    unb=unbalancedForce()
    if unb<0.01:
      break
  file.write(str(pg)+" "+str(unsat.getSaturation(False))+" "+str(triax.strain[1]-ei1)+"\n")
file.close()
