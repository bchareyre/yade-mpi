#!/usr/bin/python
# -*- coding: utf-8 -*-

'''	The script shows how to include the effect of buoyancy in a particle assembly 
	under condition of an increasing water-level. Water-level at start of the sim-
	ulation is at the lower boundary of the model. During the calculation particles
	gets partly surrounded by water and buoyancy (=density*volumeOfDisplacedWater)
	is increasing until particle is completely surrounded. When particle is sur-
	rounded volumeOfDisplacedWater is equal to the volume of the particle.
	
	For calculation of buoyancy of a clump the theoretical radius 
	R = (3*mass/(4*pi*density))^1/3 
	of a sphere with clump mass and clump volume 
	V = (4*pi*R^3)/3 = mass/density
	is used. This approximation can be used for well rounded clumps.
	
	Buoyancy is included via reduced mass (=massAtStart - dm), where dm is the 
	buoyancy. Reduced mass must be corrected via density scaling for calculation
	of correct particle accelerations.'''

#define material properties:
shear_modulus			= 3.2e10
poisson_ratio			= 0.15
young_modulus			= 2*shear_modulus*(1+poisson_ratio)
angle					= 0.5	#friction angle in radians
rho_p					= 2650	#density of particles

v_iw					= 1  #velocity of increasing water-level

#model boundaries:
boundaryMin = Vector3(-1.5,-1.5,0)
boundaryMax = Vector3(1.5,1.5,2)

#material:
id_Mat=O.materials.append(FrictMat(young=young_modulus,poisson=poisson_ratio,density=rho_p,frictionAngle=angle))
Mat=O.materials[id_Mat]

#create assembly of spheres:
sp=pack.SpherePack()
sp.makeCloud(minCorner=boundaryMin,maxCorner=boundaryMax,rMean=.2,rRelFuzz=.5,num=100,periodic=False)
O.bodies.append([sphere(c,r,material=Mat) for c,r in sp])

#create template for clumps and replace 10% of spheres by clumps:
templateList = [clumpTemplate(relRadii=[1,.5],relPositions=[[0,0,0],[.7,0,0]])]
O.bodies.replaceByClumps(templateList,[.1])

#create boundary:
O.bodies.append(facetBox((0,0,1), (boundaryMax-boundaryMin)/2, fixed=True, material=Mat))#boundaryMax-boundaryMin

#define engines:
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_MindlinPhys(betan=0.0,betas=0.0,label='ContactModel')],
		[Law2_ScGeom_MindlinPhys_Mindlin(neverErase=False,label='Mindlin')]
		),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=1,timestepSafetyCoefficient=0.8, defaultDt=0.9*PWaveTimeStep(),label='ts'),
	NewtonIntegrator(gravity=(0,0,-10),damping=0.7,label='integrator')
]
ts.dead=True

#definition to apply buoyancy:
def apply_buo(water_height,saturatedList,startMass):
	for b in O.bodies:
		if b not in saturatedList:
			h_low = 1e9
			h_high = -1e9
			if b.isStandalone and isinstance(b.shape,Sphere):
				rad = b.shape.radius
				pos = b.state.pos
				h_low = pos[2] - rad
				h_high = pos[2] + rad
			elif b.isClump:				#determine rad, h_high and h_low for clumps:
				keys = O.bodies[b.id].shape.members.keys()
				for ii in range(0,len(keys)):
					pos = O.bodies[keys[ii]].state.pos
					h_low = min(h_low,pos[2]-O.bodies[keys[ii]].shape.radius)
					h_high = max(h_high,pos[2]+O.bodies[keys[ii]].shape.radius)
				rad = ( 3*startMass[b.id]/(4*pi*O.bodies[keys[0]].mat.density) )**(1./3.)		#get radius from startMass
			else:
				continue
			if water_height > h_low:
				dh = water_height - h_low
				dh = min(dh,2*rad)	#to get sure, that dh is not bigger than 2*radius
				dm = (pi/3)*dh*dh*(3*rad - dh)*1000		#buoyancy acts via reduced mass dm=rho*V_cap
				b.state.mass = startMass[b.id] - dm		#needs a list of masses of all particles at start (see 5-final.py)
				#reduced mass must be corrected via density scaling for accelerations a = F/m_reduced, a_correct = a*densityScaling, see line 179 in pkg/dem/NewtonIntegrator.cpp:
				b.state.densityScaling = (startMass[b.id] - dm)/startMass[b.id]
				if water_height > h_high:
					saturatedList.append(b)
	return saturatedList

#STEP1: reduce overlaps from replaceByClumps() method:
O.dt=1e-6 #small time step for preparation steps via calm()
print '\nSTEP1 in progress. Please wait a minute ...\n'
O.engines=O.engines+[PyRunner(iterPeriod=10000,command='calm()',label='calmRunner')]
O.run(1000000,True)
calmRunner.dead=True

#STEP2: let particles settle down:
O.dt=1e-5
print '\nSTEP2 in progress. Please wait a minute ...\n'
O.run(50000,True)

#get maximum body id:
idmax=0
for b in O.bodies:
	idmax=max(idmax,b.id)
integrator.densityScaling=True	#activate density scaling

#get a list of masses at start:
startMass = [0 for ii in range(0,idmax+1)]
for b in O.bodies:
	if (b.isStandalone and isinstance(b.shape,Sphere)) or b.isClump:
		startMass[b.id] = b.state.mass

#start PyRunner engine to apply buoyancy:
saturatedList = []
t0 = O.time
O.engines=O.engines+[PyRunner(iterPeriod=100,command='saturatedList=apply_buo(((O.time-t0) * v_iw),saturatedList,startMass)',label='buoLabel')]

#create 2 facets, that show water height:
idsWaterFacets =  []
idsWaterFacets.append(O.bodies.append(facet( \
			[ boundaryMin, [boundaryMax[0],boundaryMin[1],boundaryMin[2]], [boundaryMax[0],boundaryMax[1],boundaryMin[2]] ], \
			fixed=True,material=FrictMat(young=0),color=(0,0,1),wire=False)))#no interactions will appear
idsWaterFacets.append(O.bodies.append(facet( \
			[ [boundaryMax[0],boundaryMax[1],boundaryMin[2]], [boundaryMin[0],boundaryMax[1],boundaryMin[2]], boundaryMin ], \
			fixed=True,material=FrictMat(young=0),color=(0,0,1),wire=False)))#no interactions will appear

#set velocity of incr. water level to the facets:
for ids in idsWaterFacets:
	O.bodies[ids].state.vel = [0,0,v_iw]

#STEP3: simulate buoyancy#
O.dt=3e-5
from yade import qt
qt.Controller()
v=qt.View()
v.eyePosition=(-7,0,2); v.upVector=(0,0,1); v.viewDir=(1,0,-.1); v.axes=True; v.sceneRadius=1.9
print '\nSTEP3 started ...\n'
O.run(60000)

