# -*- coding: utf-8
# This script is used in --check to keep the functionality of adding/removal
# of particles and clumps functional

import itertools
from numpy import *


kinEnergyMax = 100000
# Parameters
tc=0.001# collision time 
en=.3  # normal restitution coefficient
es=.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
# facets material
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,tc=tc,en=en,et=es))
# default spheres material
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,tc=tc,en=en,et=es)) 

O.dt=.05*tc # time step

Rs=0.05 # particle radius

# Create geometry

x0=0.; y0=0.; z0=0.; ab=.7; at=2.; h=1.; hl=h; al=at*3

zb=z0;   x0b=x0-ab/2.; y0b=y0-ab/2.; x1b=x0+ab/2.; y1b=y0+ab/2.
zt=z0+h; x0t=x0-at/2.; y0t=y0-at/2.; x1t=x0+at/2.; y1t=y0+at/2.
zl=z0-hl;x0l=x0-al/2.; y0l=y0-al/2.; x1l=x0+al/2.; y1l=y0+al/2.

vibrationPlate = O.bodies.append(geom.facetBunker((x0,y0,z0),
  dBunker=ab*4.0,dOutput=ab*1.7,hBunker=ab*3,hOutput=ab,hPipe=ab/3.0,
  wallMask=4,segmentsNumber=10,material=facetMat))

# Create engines
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	DomainLimiter(lo=(-4,-4,-1),hi=(4,4,4),iterPeriod=5000,label='domLim'),
	NewtonIntegrator(damping=0, gravity=[0,0,-9.81]),
	PyRunner(iterPeriod=6500,command='addBodies()',nDo=7,label='addb'),
	PyRunner(iterPeriod=10000,command='state()',label='state'),
]

numSphereGen = 0
def addBodies():
	global numSphereGen
	# Create clumps...
	clumpColor=(0.0, 0.5, 0.5)
	for k,l in itertools.product(arange(0,10),arange(0,10)):
		clpId,sphId=O.bodies.appendClumped([sphere(Vector3(x0t+Rs*(k*4+2),y0t+Rs*(l*4+2),i*Rs*2+zt+ab*3),Rs,color=clumpColor,material=dfltSpheresMat) for i in xrange(4)])
		numSphereGen += len(sphId)

	# ... and spheres
	spheresColor=(0.4, 0.4, 0.4)
	for k,l in itertools.product(arange(0,9),arange(0,9)):
		sphAloneId=O.bodies.append( [sphere( Vector3(x0t+Rs*(k*4+4),y0t+Rs*(l*4+4),i*Rs*2.3+zt+ab*3),Rs,color=spheresColor,material=dfltSpheresMat) for i in xrange(4) ] )
		numSphereGen += len(sphAloneId)

def state():
	global numSphereGen
	global resultStatus
	print "Iter %d: Total number of generated spheres %d, removed particles %d, current particles %d, kinEnergy %g"%(O.iter, numSphereGen, domLim.nDeleted, numSphereGen-domLim.nDeleted, utils.kineticEnergy())
	if (utils.kineticEnergy() > kinEnergyMax):
		print "Kinetic energy is over a threshold value! Error!"
		resultStatus += 1
		O.pause()

addBodies()
O.run(10, True)
O.bodies.erase(64, True)

O.run(20002, True)
