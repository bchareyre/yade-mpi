# -*- coding: utf-8 -*-
from yade import pack
from utils import *

num_spheres=500
## corners of the initial packing
mn,mx=Vector3(0,0,0),Vector3(1,1,1)
thick = 0.01
compFricDegree = 2
rate=0.2
damp=0.1
stabilityThreshold=0.001
key='_define_a_name_'

## create material #0, which will be used as default
O.materials.append(FrictMat(young=5e6,poisson=0.5,frictionAngle=radians(compFricDegree),density=2600,label='spheres'))
O.materials.append(FrictMat(young=5e6,poisson=0.5,frictionAngle=0,density=0,label='walls'))

## create walls around the packing
walls=utils.aabbWalls([mn,mx],thickness=thick,material='walls')
wallIds=O.bodies.append(walls)

sp=pack.SpherePack()
sp.makeCloud(mn,mx,-1,0.3333,num_spheres,False, 0.95)

volume = (mx[0]-mn[0])*(mx[1]-mn[1])*(mx[2]-mn[2])
mean_rad = pow(0.09*volume/num_spheres,0.3333)

clumps=False
if clumps:
	c1=pack.SpherePack([((-0.2*mean_rad,0,0),0.5*mean_rad),((0.2*mean_rad,0,0),0.5*mean_rad)])
	sp.makeClumpCloud((0,0,0),(1,1,1),[c1],periodic=False)
	O.bodies.append([utils.sphere(center,rad,material='spheres') for center,rad in sp])
	standalone,clumps=sp.getClumps()
	for clump in clumps:
		O.bodies.clump(clump)
		for i in clump[1:]: O.bodies[i].shape.color=O.bodies[clump[0]].shape.color
	#sp.toSimulation()
else:
	O.bodies.append([utils.sphere(center,rad,material='spheres') for center,rad in sp])

O.dt=.5*utils.PWaveTimeStep() # initial timestep, to not explode right away
O.usesTimeStepper=True

triax=ThreeDTriaxialEngine(
	maxMultiplier=1.005,
	finalMaxMultiplier=1.002,
	thickness = thick,
	stressControl_1 = False,
	stressControl_2 = False,
	stressControl_3 = False,
	## The stress used for (isotropic) internal compaction
	sigma_iso = 10000,
	## Independant stress values for anisotropic loadings
	sigma1=10000,
	sigma2=10000,
	sigma3=10000,
	internalCompaction=True,
	Key=key,
)

newton=NewtonIntegrator(damping=damp)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()],verletDist=-mean_rad*0.06),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8, defaultDt=4*utils.PWaveTimeStep()),
	triax,
	TriaxialStateRecorder(iterPeriod=100,file='WallStresses'+key),
	newton
]

#Display spheres with 2 colors for seeing rotations better
Gl1_Sphere.stripes=0
yade.qt.Controller(), yade.qt.View()

while 1:
  O.run(1000, True)
  #the global unbalanced force on dynamic bodies, thus excluding boundaries, which are not at equilibrium
  unb=unbalancedForce()
  #average stress
  #note: triax.stress(k) returns a stress vector, so we need to keep only the normal component
  meanS=(triax.stress(triax.wall_right_id)[0]+triax.stress(triax.wall_top_id)[1]+triax.stress(triax.wall_front_id)[2])/3
  print 'unbalanced force:',unb,' mean stress: ',meanS
  if unb<stabilityThreshold and abs(meanS-triax.sigma_iso)/triax.sigma_iso<0.001:
    break

O.save('compressedState'+key+'.xml')
print "###      Isotropic state saved      ###"

#let us turn internal compaction off...
triax.internalCompaction=False

#
triax.setContactProperties(30)

#... and make stress control independant on each axis
triax.stressControl_1=triax.stressControl_2=triax.stressControl_3=True
# We have to turn all these flags true, else boundaries will be fixed
triax.wall_bottom_activated=True
triax.wall_top_activated=True
triax.wall_left_activated=True
triax.wall_right_activated=True
triax.wall_back_activated=True
triax.wall_front_activated=True


#If we want a triaxial loading at imposed strain rate, let's assign srain rate instead of stress
triax.stressControl_2=0 #we are tired of typing "True" and "False", we use implicit conversion from integer to boolean
triax.strainRate2=0.01
triax.strainRate1=triax.strainRate3=1000.0

#Else if we want imposed stress increments, etc...


##First perform a transverse isotropic compression (or a stress controlled drained triaxial compression) to reach an initial state from where stress probes will be applied
##... need to active stress control in 3 directions
#triax.stressControl_1=triax.stressControl_2=triax.stressControl_3=True
##... choose the value of axial stress where we want to stop the compression
#triax.sigma2=12000
##... fix a maximum strain rate to go progressivly to the desired stress state in direction 2
#triax.strainRate2=0.01
##... fix a high value of maximum strain rate in radial direction to be sure to keep in any conditions a constant confining pressure
#triax.strainRate1=triax.strainRate3=1000.0


#while 1:
  #O.run(100, True)
  ##the global unbalanced force on dynamic bodies, thus excluding boundaries, which are not at equilibrium
  #unb=unbalancedForce()
  ##note: triax.stress(k) returns a stress vector, so we need to keep only the normal component
  #axialS=triax.stress(triax.wall_top_id)[1]
  #print 'unbalanced force:',unb,' sigma2: ',axialS
  #if unb<stabilityThreshold and abs(axialS-triax.sigma2)/triax.sigma2<0.001:
    #break

#O.save('anisotropicState'+key+'.xml')


##Perform stress probes from the anisotropic state

#dSnorm = 100.0 #norm of the stress increment
#nbProbes = 16 #number of stress directions tested
#rampIte = 20 #nb iterations to increase the stress state until the final desired stress value
##LUC: je fixe des nombres d'iterations c'est moins elegant que de chercher explicitement un etat d'equilibre mais ca permet de poursuivre le calcul meme si un etat de contrainte n'est pas correctement atteint pour un stress probe et qu'il est difficile de se stabiliser a cet etat de contrainte (i.e. attendre longtemps...)
#stabIte = 5000 #nb iterations to stabilize sample after reaching the final stress value


## an array for saving stress increments and strain responses; arrays are in "numpy" extension
#import numpy
#probings=numpy.zeros((3,nbProbes))

#def increment(dsr=0,dsa=1):
	#for ite in range(rampIte):# progressivaly increase of stress state
		#O.run(20, True)
		##incrementation of stress state
		#triax.sigma2 = initSa+dsa/rampIte*ite
		#triax.sigma1 = triax.sigma3 = initSr+dsr/rampIte*ite
		#print triax.sigma1, triax.sigma2

	## fix the stress value for stabilization at the final state
	#triax.sigma2 = initSa+dsa
	#triax.sigma1 = triax.sigma3 = initSr+dsr

	#while 1:
		#O.run(100, True)
		#unb=unbalancedForce()
		#print 'unbalanced force:',unb,' strain: ',triax.strain
		#if unb<stabilityThreshold: break



## loop over all the stress directions
#for i in range(nbProbes):

	## computation of the stress direction of the current stress probe
	#alphaS = 2*pi/nbProbes*(i-1)
	#print 'stress probe nb:',i,' stress direction (deg): ',degrees(alphaS)

	## computation of the stress increment in the axial direction
	#dSa = dSnorm*sin(alphaS) 

	## computation of the stress increment in the radial direction
	#dSr = dSnorm*cos(alphaS)/sqrt(2.0)

	##Load the initial anisotropic state before running a new stress probe
	#O.load('anisotropicState'+key+'.xml')
	##We redefine the "triax" label, else it would point to inactive engine from previous simulation that is still in memory
	#triax=O.engines[4]

	#initSa=triax.sigma2  #save of the initial axial stress
	#initSr=triax.sigma1  #save of the initial radial stress

	## define the final stress state to be reached
	#finalSa = initSa+dSa
	#finalSr = initSr+dSr

	##... need to active stress control in 3 directions if not yet done
	#triax.stressControl_1=triax.stressControl_2=triax.stressControl_3=True

	## fix a high value of maximum strain rate, the progressive loading will be done by progressively increasing the desired stress state at each iteration
	#triax.strainRate1=triax.strainRate2=triax.strainRate3=1000.0

	#increment(dSr,dSa)
	#probings[:,i]=triax.strain
	


##open a file for writing probing results
#a=open('probings'+key,'w')
#for i in range(nbProbes): a.write(str(probings[0][i])+' '+str(probings[1][i])+' '+str(probings[2][i])+'\n')
#a.close()

##plot
#from pylab import *
#plot(probings[0,:]*sqrt(2),probings[1,:],'bo--')
#ylabel(r'$\epsilon_{22}$')
#xlabel(r'$\epsilon_{11} \times \sqrt{2}$')
#title('response envelop')
#grid()

#show()

