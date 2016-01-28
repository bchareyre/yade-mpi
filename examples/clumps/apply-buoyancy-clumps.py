!/usr/bin/python
# -*- coding: utf-8 -*-

'''	The script shows how to include the effect of buoyancy in a particle assembly 
	under condition of an increasing water-level. Water-level at start of the sim-
	ulation is at the lower boundary of the model. During the calculation particles
	get partly surrounded by water and buoyancy (=fluidDensity*volumeOfDisplacedWater)
	is increasing until particle is completely surrounded. When particle is sur-
	rounded volumeOfDisplacedWater is equal to the volume of the particle.
	
	For calculation of buoyancy of a clump the equivalent radius 
	R = (3*clumpMass/(4*pi*particleDensity))^1/3 
	of a sphere with clump mass and clump volume 
	V = (4*pi*R^3)/3 = clumpMass/particleDensity
	is used. This approximation can be used for well rounded clumps.
	
	Buoyancy is included with an additional force 
	F_buo = -volumeOfDisplacedWater*fluidDensity*gravityAcceleration.'''

#define material properties:
shearModulus			= 3.2e10
poissonRatio			= 0.15
youngModulus			= 2*shearModulus*(1+poissonRatio)
angle					= 0.5	#friction angle in radians
rho_p					= 2650	#density of particles
rho_f					= 5000	#density of the fluid			rho_f > rho_p = floating particles

v_iw					= 1  #velocity of increasing water-level

#model boundaries:
boundaryMin = Vector3(-1.5,-1.5,0)
boundaryMax = Vector3(1.5,1.5,2)

#define colors:
sphereColor = (.8,.8,0.)#dirty yellow
clumpColor	= (1.,.55,0.)#dark orange
boxColor	= (.1,.5,.1)#green
waterColor	= (.2,.2,.7)#blue

#material:
id_Mat=O.materials.append(FrictMat(young=youngModulus,poisson=poissonRatio,density=rho_p,frictionAngle=angle))
Mat=O.materials[id_Mat]

#create assembly of spheres:
sp=pack.SpherePack()
sp.makeCloud(minCorner=boundaryMin,maxCorner=boundaryMax,rMean=.2,rRelFuzz=.5,num=100,periodic=False)
O.bodies.append([sphere(c,r,material=Mat,color=sphereColor) for c,r in sp])

#create template for clumps and replace 10% of spheres by clumps:
templateList = [clumpTemplate(relRadii=[1,.5],relPositions=[[0,0,0],[.7,0,0]])]
O.bodies.replaceByClumps(templateList,[.1])
#color clumps:
for b in O.bodies:
	if b.isClumpMember:
		b.shape.color=clumpColor

#create boundary:
O.bodies.append(geom.facetBox((0,0,1), (boundaryMax-boundaryMin)/2, fixed=True, material=Mat, color=boxColor))

#define engines:
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_MindlinPhys()],
		[Law2_ScGeom_MindlinPhys_Mindlin(neverErase=False)]
		),
	NewtonIntegrator(gravity=(0,0,-10),damping=0.7,label='integrator')
]

#definition to apply buoyancy:
def applyBuoyancy():
	global waterLevel
	waterLevel = (O.time-t0) * v_iw + boundaryMin[2]
	for b in O.bodies:
		zMin = 1e9
		zMax = -1e9
		if b.isStandalone and isinstance(b.shape,Sphere):
			rad = b.shape.radius
			zMin = b.state.pos[2] - rad
			dh = min((waterLevel - zMin),2*rad)	#to get sure, that dh is not bigger than 2*radius
		elif b.isClump:				#determine rad, zMin and zMax for clumps:
			for ii in b.shape.members.keys():
				pos = O.bodies[ii].state.pos
				zMin = min(zMin,pos[2]-O.bodies[ii].shape.radius)
				zMax = max(zMax,pos[2]+O.bodies[ii].shape.radius)
			#get equivalent radius from clump mass:
			rad = ( 3*b.state.mass/(4*pi*O.bodies[b.shape.members.keys()[0]].mat.density) )**(1./3.)		
			#get dh relative to equivalent sphere, but acting when waterLevel is between clumps z-dimensions zMin and zMax:
			dh = min((waterLevel - zMin)*2*rad/(zMax - zMin),2*rad)		
		else:
			continue
		if dh > 0:
			F_buo = -1*(pi/3)*dh*dh*(3*rad - dh)*rho_f*integrator.gravity										# = -V*rho*g
			O.forces.setPermF(b.id,F_buo)

#STEP1: reduce overlaps from replaceByClumps() method:
O.dt=1e-6 #small time step for preparation steps via calm()
print '\nSTEP1 in progress. Please wait a minute ...\n'
O.engines=O.engines+[PyRunner(iterPeriod=10000,command='calm()',label='calmRunner')]
O.run(100000,True)

#STEP2: let particles settle down
calmRunner.dead=True
O.dt=2e-5
print '\nSTEP2 in progress. Please wait a minute ...\n'
O.run(50000,True)

#start PyRunner engine to apply buoyancy:
t0 = O.time
waterLevel = boundaryMin[2]
O.engines=O.engines+[PyRunner(iterPeriod=100,command='applyBuoyancy()',label='buoLabel')]

#create 2 facets, that show water height:
idsWaterFacets =  []
idsWaterFacets.append(O.bodies.append(facet( \
			[ boundaryMin, [boundaryMax[0],boundaryMin[1],boundaryMin[2]], [boundaryMax[0],boundaryMax[1],boundaryMin[2]] ], \
			fixed=True,material=FrictMat(young=0),color=waterColor,wire=False)))#no interactions will appear
idsWaterFacets.append(O.bodies.append(facet( \
			[ [boundaryMax[0],boundaryMax[1],boundaryMin[2]], [boundaryMin[0],boundaryMax[1],boundaryMin[2]], boundaryMin ], \
			fixed=True,material=FrictMat(young=0),color=waterColor,wire=False)))#no interactions will appear

#set velocity of incr. water level to the facets:
for ids in idsWaterFacets:
	O.bodies[ids].state.vel = [0,0,v_iw]

#STEP3: simulate buoyancy with increasing water table condition
O.dt=3e-5
from yade import qt
qt.Controller()
v=qt.View()
v.eyePosition=(-7,0,2); v.upVector=(0,0,1); v.viewDir=(1,0,-.1); v.axes=True; v.sceneRadius=1.9
print '\nSTEP3 started ...\n'

O.run(70000)
